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

#ifndef __PLATFORM_X64_MAP_CREATOR_X64_H__
#define __PLATFORM_X64_MAP_CREATOR_X64_H__

#include "phys-region-list-x64.h"
#include <cstring>

namespace OS {

const int MaximumAllocators = 0x10;
typedef ANAlloc::BBTree TreeType;
typedef ANAlloc::AllocatorList<MaximumAllocators, TreeType> AllocatorList;

class MapCreator {
private:
  /**
   * Starts at end of kernel section and increments by a page each time a new
   * page table is allocated. There may be jumps in this value when jumping to
   * a new region of physical memory.
   */
  uintptr_t physOffset;
  
  /**
   * Starts at end of kernel section and always increments by one page for each
   * page table allocation.
   */
  uintptr_t virtOffset;
  
  /**
   * The amount of virtual memory which has been mapped to physical memory.
   * This will go up by 2MB at a time (currently).
   */
  uintptr_t virtMapped;
  
  /**
   * Starts out as false. Once virtMapped > virtOffset + 0x4000 this will
   * become true, cr3 will be reloaded with the new mappings, and we will start
   * using virtOffset instead of physOffset to modify page tables.
   */
  bool hasSwitched;
  
  PhysAddr pml4;
  PhysAddr pdpt;
  
  PhysRegionList * regions;
  AllocatorList * allocators;
  
  // before hasSwitched, this is used
  uint64_t * physPDT;
  uint64_t * virtPDT;
  
  uint64_t * virtScratchPT;
  
  int pdtOffset;
  int pdptOffset;
  
  void IncrementPhysOffset();
  void AddPhysOffset(size_t size);
  
  void InitializeTables();
  void MapNextPage();
  void Switch();
  void AllocatePage(uint64_t ** phys, uint64_t ** virt);
  
  uint64_t * VisiblePDT();
  uint64_t * VisiblePDPT();
  
public:
  MapCreator(PhysRegionList *, AllocatorList *);
  
  /**
   * @param total The total amount of memory to map. This must be a multiple
   * of 2MB.
   * @param current The amount of linear memory which is already reserved.
   * This must be a multiple of 2MB.
   */
  void Map(uintptr_t total, uintptr_t current);
  
  /**
   * Returns the "scratch" page table at the end of kernel address space, used
   * for quickly mapping in physical memory in order to access it.
   */
  uint64_t * ScratchPageTable();

  /**
   * Returns the physical PML4.
   */
  PhysAddr GetPML4();

  /**
   * Returns the kernel PDPT.
   */
  PhysAddr GetPDPT();

};

}

#endif

