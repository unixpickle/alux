# Conceptual overview

The alux memory model in the kernel is layered. Here is a description of how it works.

## General abstractions

The platform-independent part of the kernel defines (but does not implement) an interface for allocating chunks of physically contiguous memory, and for mapping those chunks in a manner similar to `mmap`. This abstraction, however, leaves platform dependent code in charge: it makes no requirements about the location of physical memory, or of the virtual memory it gets mapped to.

## x64 physical map

When the x86-64 kernel is loaded by a multiboot-compatible bootloader, it immediately creates a 1GiB identity map before switching to 64-bit Long Mode. At the beginning of this is 1MiB reserved for BIOS information, and then a certain small amount of data reserved for the kernel (currently, this is 16MiB, but it will be dynamic in the future).

The kernel uses it's 1GiB identity map to generate enough page tables to map the entire kernel into a new address space. This generally involves allocating 3 pages (PML4, PDPT, and PDT containing 2MiB pages).

Before switching to the new page tables, the kernel maps does one last thing to the page tables. It creates a new page table (level 4) which controls the region right before the 512GiB mark. This page table is stored as part of the kernel section of data so that it is guaranteed to be mapped in. This is the *scratch* page table.

Now, the kernel switches to the new page tables. The kernel can modify the *scratch* page table as needed in order to read/write pages of physical memory. At any given moment, the kernel may access 0x200 pages of physical memory that are not mapped. This is extremely useful for modifying page tables without something like fractal mapping (which is pointless for per-process page tables where you want to modify the page table without switching to it).

## x64 physical allocator

We have the physical mapping down, but in order to map new addresses we may need to allocate new page tables, PDTs, etc. Additionally, the general abstractions which we are trying to satisfy call for some sort of physical allocator. In order to implement this, we will use [analloc2](https://github.com/unixpickle/analloc2).

I created the analloc2 project with OS development in mind. Because of this, it has almost everything we need. The most important feature it has is the topological allocator. You pass this allocator a list of physical memory regions, and it builds the structures needed to efficiently allocate aligned memory from these regions.

In order to setup analloc2, we need to create a contiguous buffer in virtual memory. This buffer needs to be large enough to store the allocators' information bitmaps. First, we setup a dummy physical allocator for the memory mapper to use. This allocator starts at a given physical address and progresses by a page at a time, skipping gaps as they come up, and making sure to return aligned addresses.

Now, we use the dummy allocator and the physical mapper to map enough *extra* physical memory into virtual memory for analloc2's topological allocator. Finally, we setup analloc2, and set the physical mapper to use our new, *real* allocator.


# Code-structure overview

This will be an overview of the various classes that exist and how they are used for x86-64 memory management.

## class *PageAllocator*

This is an abstract base class used by the `KernelMap` and `MapSetup` classes for page allocation. Essentially, a `PageAllocator` allocates aligned pages of physical memory.

## file *size-config-x64.hpp*

Defines:

* `KernelDataSize()` returns the number of bytes that are reserved at the beginning of the address space. This includes the 1MiB reserved for BIOS data.
* `ScratchPTCount` the number of scratch page tables to use. This must be less than 0x200, but should be at least 1. There will be `0x200 * ScratchPTCount` scratch pages available to the system at any given time.
* `ScratchPTStart()` returns a pointer to a static variable within the kernel. This static variable contains enough space for `0x1000 * ScratchPTCount` bytes, and is page aligned. This is where the scratch page tables are stored.

## class *MapSetup*

This class uses a `PageAllocator` to allocate the page tables and structures needed at boot in order to map in the kernel. It does *not* map in scratch page tables.

## class *KernelMap*

This class handles scratch pages and mapping of memory regions. Additionally, it configures the scratch and page table structures initially, and invokes a `MapSetup`. A global instance of this class (returned by `KernelMap::GetGlobal()`) is used for all paging abstractions throughout the runtime of the kernel.

## class *TableMgr*

Since both kernel page tables and user page tables need to manipulate page table structures directly, it only makes sense to abstract this job out to some other class. The `TableMgr` class manipulates a virtual mapping given the table's PML4. It (indirectly) uses the `KernelMap`'s scratch to read and write physical structures.

## class *StepAllocator*

This is a direct subclass of `PageAllocator`. It allocates memory linearly and increments an address for the highest used address. Additionally, the `StepAllocator` class has a method `AllocSize(size_t)`, method to allocate a certain number of aligned bytes.

The `PhysicalAllocator` class uses `StepAllocator`s as the "dummy" allocator for the `KernelMap` until the *real* physical allocator has been configured.

## class *PhysicalAllocator*

This class manages the analloc2 topological allocator. It is a subclass of `PageAllocator`, and thus may be used by `KernelMap` after configuration is complete. A global instance of the physical allocator may be obtained with `PhysicalAllocator::GetGlobal()`, but most of its facilities may be accessed using the platform-independent abstractions.

## file *scope-scratch-x64.hpp*

C++ stack variables are awesome. Especially the fact that a destructor is called when a class instance goes out of scope. While the raw `KernelMap` scratch system is acceptable, it is much safer to use stack-based scratch management.

### class *ScopeScratch*

This class maps a physical address into scratch memory upon initialization, and unmaps it upon destruction. Additionally, it provides a `Reassign()` method so you may reuse the same scratch address for a different physical page.

### class *TypedScratch&lt;T&gt;*

This is a subclass of `ScopeScratch`. The only addition is that you may access it using the `[]` operator. The `TypedScratch<T>` object acts like a C-style array of type `T`.

## file *phys-memcpy-x64.hpp*

This provides a wrapper for the scratch mechanism. The following functions are useful when you may need to access multiple pages of physical memory:

	void MemcpyToPhys(PhysAddr phys, VirtAddr virt, size_t size);
	void MemcpyToVirt(VirtAddr virt, PhysAddr phys, size_t size);
