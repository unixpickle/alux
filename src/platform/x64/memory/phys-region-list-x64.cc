#include "phys-region-list-x64.hpp"

namespace OS {

namespace x64 {

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
  
  // DEBUG: these regions test the OS's ability to deal with random-ass memory
  // configurations
  /*
  MemoryRegion r1(0, 0x1000003);
  MemoryRegion r2(0x2000000, 0x3000);
  MemoryRegion r3(0x3000000, 0x10000);
  MemoryRegion r4(0x3200000, 0x200000);
  MemoryRegion r5(0x3400000, 0x600000);
  regions[0] = r1;
  regions[1] = r2;
  regions[2] = r3;
  regions[3] = r4;
  regions[4] = r5;
  regionCount = 5;
  */
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
  uintptr_t idx = ((uintptr_t)reg - (uintptr_t)regions)
    / sizeof(MemoryRegion);
  assert(idx < (uintptr_t)regionCount);
  if (idx + 1 == (uintptr_t)regionCount) return NULL;
  return reg + 1;
}

}

}
