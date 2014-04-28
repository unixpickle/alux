/*
 * Copyright (c) 2014, Alex Nichol and Alux contributors.
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __PLATFORM_VM_H__
#define __PLATFORM_VM_H__

#include <cstdint>
#include <cstddef>
#include <memory/region.h>

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
  virtual void * Allocate(uint64_t size, size_t alignment) = 0;
  
  /**
   * Free a buffer by passing its virtual address.
   */
  virtual void Free(void *) {
  }
  
  /**
   * Return the physical address of a virtual address that was just allocated.
   * The virtual address is really an address in the current page mapping,
   * but the object using this instance might not know that.
   */
  virtual void * PhysicalAddress(void * virt) = 0;
  
  /**
   * Returns the virtual address for a physical one.
   */
  virtual void * VirtualAddress(void * phys) = 0;
};

class VirtualMapping {
public:
  /**
   * Returns the number of page sizes supported by the platform.
   */
  static int NumPageSizes();
  
  /**
   * Returns the list of page sizes in ascending order.
   */
  static uintptr_t * PageSizes();
  
  /**
   * For each element in the page sizes array, the corresponding one in the
   * PhysicalAlignments array indicates the alignment that physical pieces
   * of memory must have when mapped using a certain page size.
   */
  static uintptr_t * PhysicalAlignments();
  
  /**
   * For each element in the page sizes array, the corresponding one in the
   * VirtualAligments array indicates the alignment that a virtual address must
   * have in order to map it with a given page size.
   */
  static uintptr_t * VirtualAlignments();
  
  /**
   * Adds a pointer to an integer and returns the result. This is needed in
   * the case of canonical-form addresses.
   */
  static void * AddPtr(void * ptr1, uintptr_t val);
  
  /**
   * Returns a list of mappable virtual memory regions.
   */
  static MemoryRegion * VirtualMemoryRegions();
  
  /**
   * Returns the number of non-contiguous chunks of mappable addresses.
   */
  static int VirtualMemoryRegionCount();
  
  /**
   * Create a new VirtualMapping with nothing mapped yet.
   */
  static VirtualMapping * NewMappingIP(PhysicalAllocator * allocator,
                                       void * base);
  
  /**
   * Returns the size of a new virtual in-place mapping.
   */
  static intptr_t IPMappingSize();
  
  /**
   * Create a new mapping which will build off of an existing VirtualMapping.
   * @param allocator The physical allocator to use.
   * @param mapping The virtual address map to build on. This mapping must stay
   * allocated for the duration of the returned mapping's lifetime. At least
   * some of the contents of this mapping should be *referenced* instead of
   * copied in order to provide for less memory overhead.
   */
  static VirtualMapping * NewSubmapping(PhysicalAllocator * allocator,
                                        VirtualMapping * mapping);
  
  /**
   * Returns the number of bytes which should stand between the existing
   * mapping (passed to NewSubmapping()) and new addresses being mapped.
   */
  static uintptr_t BestDivide();
  
  enum PageFlags {
    PageFlagExecute = 1 << 0,
    PageFlagGlobal = 1 << 1,
    PageFlagUser = 1 << 2
  };
  
  /**
   * Free all resources associated with this virtual mapping.
   */
  virtual ~VirtualMapping() {
  }

  virtual void SetAllocator(PhysicalAllocator *) {
  }

  /**
   * Unmap the page table entry containing a virtual address.
   */
  virtual void Unmap(void *) {
  }
  
  /**
   * Map a virtual address to a physical address.
   * @return false if something is already mapped between address and
   * address+size.
   */
  virtual bool Map(void * address, void * phys, uint64_t size, int flags) = 0;
  
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
  virtual bool Lookup(void * address,
              void ** phys,
              void ** start,
              int * flags,
              uint64_t * size) = 0;

  /**
   * Start using this mapping on the current CPU.
   */
  virtual void MakeCurrent() {}
};

}

#endif
