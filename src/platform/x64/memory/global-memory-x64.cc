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

#include "global-memory-x64.h"

namespace OS {

static GlobalMap map;

GlobalMap & GlobalMap::GetGlobalMap() {
  return map;
}

void GlobalMap::InitializeGlobalMap(void * mbootPtr) {
  new(&map) GlobalMap(mbootPtr);
}

void GlobalMap::AddRegion(MemoryRegion & region) {
  if (regionCount == 0x40) {
    Panic("GlobalMap::AddRegion() - region overflow");
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

void AddDescription(AllocatorDescription & desc) {
  if (descriptionCount == 0x80) {
    Panic("GlobalMap::AddDescription() - description overflow");
  }
  descriptions[descriptionCount++] = desc;
}

GlobalMap::GlobalMap(void * mbootPtr) {
  regionCount = allocatorCount = descriptionCount = 0;
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
    
    MemoryRegion region((void *)start, len);
    AddRegion(region);
  }
  if (!regionCount) Panic("GlobalMap() - no regions found");
}

uintptr_t GlobalMap::VirtualUsed() {
  return 0x1000000; // TODO: use symbol here
}

void GlobalMap::GenerateDescriptions() {
  // TODO: make an ANAlloc API for this :'(
}

size_t GlobalMap::InitialBytes() {
  size_t size = (size_t)VirtualUsed();
  for (int i = 0; i < descriptionCount; i++) {
    size += ANAlloc::BBTree::MemorySize(descriptions[i].depth);
  }
  if (size & 0xfff) {
    size += 0x1000 - (size & 0xfff);
  }
  return size;
}

size_t GlobalMap::RequiredPhysical() {
  size_t size = InitialBytes();
  
  size_t ptCount = 1;
  size_t pdtCount = (size >> 21) + (size % (1L << 21) ? 1L : 0L);
  size_t pdptCount = (size >> 30) + (size % (1L << 30) ? 1L : 0L);
  size_t pml4Count = (size >> 39) + (size % (1L << 39) ? 1L : 0L);
  assert(pml4Count == 1);
  assert(pdptCount < 0x200);
  
  // we will also have a final PDT with a page table for temporary pages
  pdptCount++;
  pdtCount++;
  
  return size + (ptCount + pdtCount + pdptCount + pml4Count) * 0x1000;
}

void GlobalMap::Setup() {
  cout << "TODO: create an initial mapping!" << endl;
}

}