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

#include "phys-region-list-x64.h"

namespace OS {

void PhysRegionList::AddRegion(MemoryRegion & region) {
  if (regionCount == MaximumPhysicalRegions) {
    Panic("PhysRegionList::AddRegion() - region overflow");
  }
  
  int insertIndex = regionCount;
  for (int i = 0; i < regionCount; i++) {
    if (regions[i].GetStart() > region.GetStart()) {
      insertIndex = i;
      break;
    }
  }
  for (int i = regionCount; i > insertIndex; i--) {
    regions[i] = regions[i - 1];
  }
  regions[insertIndex] = region;
  regionCount++;
}

PhysRegionList::PhysRegionList(void * mbootPtr) {
  regionCount = 0;
  MultibootBootInfo * multibootPtr = (MultibootBootInfo *)mbootPtr;
  
  // loop through and generate the regions
  uint32_t mmapLen = multibootPtr->mmap_length;
  uint32_t mmapAddr = multibootPtr->mmap_addr;
  if (mmapAddr + mmapLen > 0x100000) {
    Panic("GRUB memory map is placed beyond 1MB -- might be destroyed!");
  }
  
  uintptr_t longAddr = (uintptr_t)mmapAddr;
  MultibootMmapInfo * info = NULL;
  MultibootMmapInfo * next = (MultibootMmapInfo *)longAddr;
  while (mmapLen > 0) {
    info = next;
    next = (MultibootMmapInfo *)((uintptr_t)info + info->size + 4);
    mmapLen -= info->size + 4;
    
    if (info->type != 1) continue;
    
    // simplify the process by assuming that the first MB is contiguous (even)
    // though really it's not at all.
    if (info->base_addr + info->length <= 0x100000) continue;
    
    // get the range
    uint64_t start = info->base_addr;
    uint64_t len = info->length;
    
    // if this region is in the lower MB, we will stretch it out to the
    // beginning of the address space.
    if (start <= 0x100000) {
      len += start;
      start = 0;
    }
    
    MemoryRegion region((uintptr_t)start, (size_t)len);
    AddRegion(region);
  }
  if (!regionCount) Panic("GlobalMap() - no regions found");
  
  /**
   * Setup test regions. This is useful only when you have at least 0x3010000
   * bytes of contiguous physical memory at the beginning of your address space.
   * This test essentially sees if the MapCreator can properly work with
   * fragmented memory.
   */
  /** TEST **
  MemoryRegion r1(0, 0x1001003); // 1 page for first allocation, weird alignment
  MemoryRegion r2(0x2000000, 0xfff);
  MemoryRegion r3(0x3000000, 0x10000);
  regions[0] = r1;
  regions[1] = r2;
  regions[2] = r3;
  regionCount = 3;
  ** END TEST **/
}

MemoryRegion * PhysRegionList::GetRegions() {
  return regions;
}

int PhysRegionList::GetRegionCount() {
  return regionCount;
}

MemoryRegion * PhysRegionList::FindRegion(uintptr_t ptr) {
  for (int i = 0; i < regionCount; i++) {
    if (regions[i].GetStart() > ptr) continue;
    if (regions[i].GetEnd() <= ptr) continue;
    return &regions[i];
  }
  return NULL;
}

MemoryRegion * PhysRegionList::NextRegion(MemoryRegion * reg) {
  uintptr_t idx = ((uintptr_t)reg) / sizeof(void *);
  if (idx + 1 == (uintptr_t)regionCount) return NULL;
  return reg + 1;
}

}

