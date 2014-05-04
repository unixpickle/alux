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

#ifndef __PLATFORM_MEMORY_H__
#define __PLATFORM_MEMORY_H__

namespace OS {

typedef uintptr_t PhysAddr;
typedef uintptr_t VirtAddr;

/**
 * Allocate a physically-contiguous chunk of physical memory.
 * @param size The minimum number of bytes to allocate
 * @param addr On success, this is the beginning of the physical buffer.
 * @return true on success
 */
bool PhysicalAlloc(size_t size, PhysAddr & addr);

/**
 * Like PhysicalAlloc(), but the resulting `addr` is a multiple of `align`.
 */
bool PhysicalAlign(size_t size, size_t align, PhysAddr & addr);

/**
 * Frees an address allocated with PhysicalAlign() or PhysicalAlloc().
 */
void PhysicalFree(PhysAddr addr, size_t size);

/**
 * Represents an application's memory map. This memory map includes a section
 * of virtual memory which is shared between every task for the kernel.
 */
class MemoryMap {
public:
  static int PageSizeCount();
  static size_t * PageSizes();
  
  /**
   * Create a new memory map. This will allocate everything it needs to.
   */
  MemoryMap();
  
  /**
   * Destroy a memory map. This will free everything it needs to.
   */
  ~MemoryMap();
  
  /**
   * @param phys The beginning of the physical segment to map.
   * @param bytes The number of bytes to map. This must be a multiple of a
   * supported page-size.
   * @param addr On success, this will be the beginning of the new virtual
   * buffer that was mapped.
   * @return true on success.
   */
  bool MapKernel(PhysAddr phys, size_t bytes, VirtAddr & addr);
  
  /**
   * Same as MapKernel(), but the mapping will be local to *this* page table.
   */
  bool MapUser(PhysAddr phys, size_t bytes, VirtAddr & addr);
  
  /**
   * Same as MapUser(), but the mapped pages cannot be executed.
   */
  bool MapUserNoExec(PhysAddr phys, size_t bytes, VirtAddr & addr);
  
  /**
   * Map "lazy" addresses in the kernel space. When the page is accessed, a
   * page fault will be triggered.
   */
  bool MapKernelLazy(PhysAddr phys, size_t bytes, VirtAddr & addr);
  
  /**
   * Same as MapKernelLazy(), but for user-space local addresses.
   */
  bool MapUserReserved(PhysAddr phys, size_t bytes, VirtAddr & addr);
  
  /**
   * Unmap the pages at a virtual address. The address must be an address
   * returned by one of the Map*() functions, and the size in bytes must be
   * equal to the size passed to the Map*() function.
   */
  void Unmap(VirtAddr addr, size_t bytes);
  
  /**
   * Invalidate the page table caches for a given address extending `bytes`
   * bytes.
   */
  void InvalidateUserCache(VirtAddr addr, size_t bytes);
  
  /**
   * Same as InvalidateUserCache(), but some architectures may do this
   * differently because of global pages.
   */
  void InvalidateKernelCache(VirtAddr addr, size_t bytes);
  
};

}

#endif
