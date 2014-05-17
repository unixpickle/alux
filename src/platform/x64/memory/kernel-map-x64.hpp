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

#ifndef __PLATFORM_X64_KERNEL_MAP_X64_H__
#define __PLATFORM_X64_KERNEL_MAP_X64_H__

#include "map-setup-x64.hpp"
#include "../common-x64.hpp"
#include "table-mgr-x64.hpp"
#include <platform/failure.hpp>
#include <cassert>
#include <utilities/lock.hpp>
#include <stdlib/runtime.hpp>

namespace OS {

namespace x64 {

class KernelMap {
private:
  uint64_t * virtScratchPTs[ScratchPTCount];
  uint64_t scratchBitmaps[ScratchPTCount * 8];
  uint64_t scratchLock OS_ALIGNED(8);

  uint64_t mapLock OS_ALIGNED(8); // applies to bu and TableMgr
  
  // the "biggest unmapped" region of memory
  VirtAddr buStart;
  size_t buSize;
  TableMgr manager;

public:
  static KernelMap & GetGlobal();
  
  KernelMap();
  
  /**
   * Sets up the paging system using the current allocator
   */
  void Setup();
  
  /**
   * Sets CR3 to this map's PML4.
   */
  void Set();
  
  /**
   * Attempts to map a physical address to a virtual address. If the map fails,
   * (VirtualAddr)0 is returned.
   */
  VirtAddr Map(PhysAddr start, size_t size, bool largePages);

  /**
   * Map a physical address to a specified virtual address. If something was
   * already mapped there, this will Panic().
   */
  void MapAt(VirtAddr virt, PhysAddr start, size_t size, bool largePages);
  
  /**
   * Set some mapping to unpresent but keep it non-zero
   */
  void ClearMap(VirtAddr virt, size_t size);
  
  /**
   * Unmap a virtual region of memory.
   */
  void Unmap(VirtAddr virt, size_t size);

  /**
   * Temporarily map a 4K physical page into a virtual address. You should lock
   * yourself to this CPU since the caches on other CPUs may have stale entires
   * for this address.
   */
  VirtAddr AllocScratch(PhysAddr start);
  
  /**
   * Reuse the same scratch for a different address.
   */
  void ReassignScratch(VirtAddr addr, PhysAddr newAddr);
  
  /**
   * Release a virtual address returned by AllocScratch to be used elsewhere.
   */
  void FreeScratch(VirtAddr ptr);

protected:
  friend class TableMgr;
  friend class PhysicalAllocator;
  
  PageAllocator * allocator;
  
  bool CanFitRegion(size_t size, bool bigPages);

};

}

}

#endif

