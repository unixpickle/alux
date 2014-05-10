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

#ifndef __PLATFORM_X64_TABLE_MGR_X64_H__
#define __PLATFORM_X64_TABLE_MGR_X64_H__

#include "scope-scratch-x64.h"

namespace OS {

namespace x64 {

class KernelMap;

/**
 * This class handles everything that is 4-level page tables.
 */
class TableMgr {
protected:
  KernelMap * kernMap;
  PhysAddr pml4;

public:
  TableMgr(KernelMap * map, PhysAddr pml4);
  TableMgr();
  
  TableMgr & operator=(const TableMgr & mgr);
  
  /**
   * mov cr3, MY_PML4
   */
  void Set();
  
  /**
   * Create a map. This will Panic() if something is already mapped where the
   * region is requested.
   */
  void Map(VirtAddr virt, PhysAddr phys, size_t size, bool largePages,
           uint64_t entryMask, uint64_t tablesMask);
  
  /**
   * Unmap a region or Panic().
   */
  void Unmap(VirtAddr virt, size_t size);
  
  /**
   * Maps a 4K page.
   */
  void MapSmallPage(VirtAddr virt,
                    PhysAddr phys,
                    uint64_t entryMask,
                    uint64_t tableMask);
  
  /**
   * Maps a 2MB page.
   */
  void MapLargePage(VirtAddr virt,
                    PhysAddr phys,
                    uint64_t entryMask,
                    uint64_t tableMask);
  
  /**
   * Unmap the page starting at addr. The thing is, this page could have been
   * 2MB, or 1GB, or plain old 4K. To deal with this, this function returns the
   * size of the page that was unmapped.
   */
  size_t UnmapPage(VirtAddr addr);
  
  /**
   * Sets the buStart and buSize fields by searching the page tables.
   */
  void FindNewBU(VirtAddr & buStart, size_t & buSize, VirtAddr maxAddr);

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
};

}

}

#endif
