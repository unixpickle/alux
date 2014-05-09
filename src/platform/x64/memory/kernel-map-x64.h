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

#include "map-setup-x64.h"

namespace OS {

namespace x64 {

class KernelMap {
private:
  PhysAddr pdpt, pml4;
  
  uint64_t * virtScratchPTs[ScratchPTCount];
  uint64_t scratchBitmaps[ScratchPTCount * 8];
  uint64_t scratchLock OS_ALIGNED(8);

  // the "biggest unmapped" region of memory
  VirtAddr buStart;
  size_t buSize;
  uint64_t mapLock OS_ALIGNED(8);

public:
  KernelMap();
  
  /**
   * Returns the first physical address which may be used. You should call
   * Set() immediately after this.
   */
  PhysAddr Setup(PhysRegionList * regs);
  
  /**
   * Sets CR3 to this map's PML4.
   */
  void Set();

  /**
   * Returns the kernel PDPT.
   */
  PhysAddr GetPDPT();
  
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
   * Temporarily map a 4K physical page into a virtual address. You should lock
   * yourself to this CPU since the caches on other CPUs may have stale entires
   * for this address.
   */
  VirtAddr AllocScratch(PhysAddr start);
  
  /**
   * Release a virtual address returned by AllocScratch to be used elsewhere.
   */
  void FreeScratch(VirtAddr ptr);

private:
  /**
   * Sets the buStart and buSize fields by searching the page tables.
   */
  void FindNewBU();
 
  /**
   * A call used by FindNewBU() to find the biggest region.
   * @param table A physical table of any depth (i.e. PML4, etc.)
   * @param depth The depth of the table (0 = PML4)
   * @param mapAddr The first virtual address this table controls.
   * @param regStart A virtual address which is controlled by this table. The
   * function looks at memory starting at this address.
   * @param contigSize On return, this is set to the number of bytes starting
   * from regStart that are unmapped.
   * @return true if the big chunk did NOT end in this table--that is, starting
   * from the entry belonging to regStart upwards, every entry contained no
   * mappings to physical memory.
   */
  bool FollowBigChunk(PhysAddr table,
                      int depth,
                      VirtAddr mapAddr,
                      VirtAddr regStart,
                      size_t & contigSize);

  /**
   * Finds the next unmapped page or region starting at a ceratin address.
   */
  bool FindNextUnmapped(PhysAddr table,
                        int depth,
                        VirtAddr mapAddr,
                        VirtAddr start,
                        VirtAddr & result);

  bool CanFitRegion(size_t size, bool bigPages);

};

}

}

#endif

