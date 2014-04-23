#ifndef __PLATFORM_VM_H__
#define __PLATFORM_VM_H__

#include <cstdint>
#include <cstddef>

namespace OS {
  
/**
* This is an abstract base class for a rudimentary physical memory allocator.
*/
class PhysicalAllocator {
public:
  /**
   * Returns a virtual address for the physical piece of memory that was
   * allocated.
   * @param alignment The returned address will be a multiple of `alignment`.
   * @discussion If the allocation cannot be performed, a panic should be
   * triggered.
   */
  virtual void * Allocate(uint64_t size, size_t alignment);
};

class VirtualMapping {
private:
  void * tableRoot;
  PhysicalAllocator * allocator;

public:
  static uintptr_t * pageSizes;
  static uintptr_t * physicalAligns;
  static uintptr_t * virtualAligns;
  static int numPageSizes;

  VirtualMapping(void * tableRoot, PhysicalAllocator * allocator) {
    this->tableRoot = tableRoot;
    this->allocator = allocator;
  }

  /**
   * Unmap the page table entry containing a virtual address.
   */
  void Unmap(void * address);
  
  /**
   * Map a virtual address to a physical address.
   * @return false if something is already mapped between address and
   * address+size.
   */
  bool Map(void * address, void * phys, uint64_t size);
  
  /**
   * Searches for an entry in the page tables corresponding to a virtual
   * address.
   * @param address The virtual address to lookup
   * @param phys Set to the physical address corresponding to the virtual 
   * address.
   * @param start Set to the physical address which marks the beginning of the
   * page table entry containing `phys`
   * @param size The number of bytes contained by this page table entry.
   * @return true if an entry was found, false otherwise.
   */
  bool Lookup(void * address, void ** phys, void ** start, uint64_t * size);

  // TODO: here, there will be smaller methods to initialize the address
  // space etc.
};

}

#endif
