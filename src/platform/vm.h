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
  
  /**
   * Free a buffer by passing its virtual address.
   */
  virtual void Free(void * ptr);
  
  /**
   * Return the physical address of a virtual address that was just allocated.
   * The virtual address is really an address in the current page mapping,
   * but the object using this instance might not know that.
   */
  virtual void * PhysicalAddress(void * virt);
  
  /**
   * Returns the virtual address for a physical one.
   */
  virtual void * VirtualAddress(void * phys);
};

class VirtualMapping {
private:
  void * tableRoot;
  PhysicalAllocator * allocator;

public:
  static int NumPageSizes();
  static uintptr_t * PageSizes();
  static uintptr_t * PhysicalAlignments();
  static uintptr_t * VirtualAlignments();
  
  enum PageFlags {
    PageFlagExecute = 1 << 0,
    PageFlagGlobal = 1 << 1,
    PageFlagUser = 1 << 2
  };

  /**
   * Create a new virtual mapping.
   */
  VirtualMapping(PhysicalAllocator * allocator);
  
  /**
   * Free all resources associated with this virtual mapping.
   */
  ~VirtualMapping();

  /**
   * Unmap the page table entry containing a virtual address.
   */
  void Unmap(void * address);
  
  /**
   * Map a virtual address to a physical address.
   * @return false if something is already mapped between address and
   * address+size.
   */
  bool Map(void * address, void * phys, uint64_t size, int flags);
  
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
  bool Lookup(void * address,
              void ** phys,
              void ** start,
              int * flags,
              uint64_t * size);

  /**
   * Start using this mapping on the current CPU.
   */
  void MakeCurrent();

  // TODO: here, there will be smaller methods to initialize the address
  // space etc.
};

}

#endif
