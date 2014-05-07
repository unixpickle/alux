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

#ifndef __PLATFORM_X64_GLOBAL_MEMORY_X64_H__
#define __PLATFORM_X64_GLOBAL_MEMORY_X64_H__

#include "map-creator-x64.h"
#include <utilities/lock.h>

namespace OS {

class GlobalMap {
private:
  PhysRegionList regions;
  AllocatorList allocators;
  PhysAddr pml4;
  PhysAddr pdpt;
  
  uint64_t * scratchTable;
  uint64_t scratchLock;
  uint64_t scratchBitmap[8];
  
  /**
   * Returns the number of bytes used by the kernel and BIOS at the beginning
   * of the virtual address space.
   *
   * This must be a multiple of 2M.
   */
  size_t MemoryForKernel();
  
  /**
   * Returns the number of bytes that the system needs for ANAlloc bitmaps.
   *
   * This must be a multiple of 4K.
   */
  size_t MemoryForBitmaps();
 
  /**
   * Returns the number of bytes that the system needs for page tables in order
   * to map all the kernel memory, bitmap memory, and memory for page tables.
   * Obviously, this method basically solves a discrete differential equation,
   * since the amount of memory used for page tables may increase given the
   * other memory needed for the page tables.
   *
   * This must be a multiple of 4K.
   */
  size_t MemoryForPageTables();
  
public:
  static GlobalMap & GetGlobalMap();
  static void InitializeGlobalMap(void * mbootPtr);
  
  // for the static variable
  GlobalMap() {}
  
  /**
   * Create a new GlobalMap, passing in the parsed regions
   */
  GlobalMap(void * mbootPtr);
  
  /**
   * Create the initial global mapping and generate physical allocators.
   */
  void Setup();
  
  /**
   * Returns the root page table for the pure kernel mapping.
   */
  PhysAddr GetPML4();
  
  /**
   * Returns the third level page table which should be placed at the beginning
   * of the PML4 in every task's address space.
   */
  PhysAddr GetPDPT();

  /**
   * Maps a physical address to a temporary address. This will automatically
   * clear the cache for the returned page. NULL will be returned if no slots
   * are available.
   *
   * @discussion Since this is a rapid change to the kernel page table, you
   * should lock yourself to one CPU before using your reserved page. That way
   * you don't get switched to a different CPU that has a stale TLB entry.
   */
  void * ReserveScratch(PhysAddr addr);

  /**
   * Release a scratch map that you made earlier.
   */
  void FreeScratchIndex(void * virt);
  
  /**
   * Returns the internal allocator list.
   */
  AllocatorList & GetAllocatorList();
  
};

}

#endif

