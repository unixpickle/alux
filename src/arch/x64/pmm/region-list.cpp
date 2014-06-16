#include <arch/x64/pmm/region-list.hpp>
#include <arch/x64/multiboot.hpp>
#include <iostream>
#include <cassert>
#include <panic>
#include <new>

namespace OS {

namespace x64 {

static RegionList globalRegions;
static MultibootBootInfo * multibootPtr;

void RegionList::InitGlobal(void * mbootPtr) {
  new(&globalRegions) RegionList();
  multibootPtr = (MultibootBootInfo *)mbootPtr;
}

RegionList & RegionList::GetGlobal() {
  return globalRegions;
}

void RegionList::Initialize() {
  cout << "Initializing region list..." << endl;
  
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
    
    MemoryRegion region(start, len);
    if (start < 0x100000000L && start + len > 0x100000000L) {
      // this will probably never happen, but if it does we need to split the
      // regions for ANAlloc to be able to differentiate between upper and
      // lower memory
      size_t lowSize = (size_t)(0x100000000L - start);
      MemoryRegion lowerRegion(start, lowSize);
      MemoryRegion upperRegion(0x100000000L, len - lowSize);
      AddRegion(lowerRegion);
      AddRegion(upperRegion);
    } else {
      AddRegion(region);
    }
  }
  if (!GetRegionCount()) {
    Panic("RegionList::Initialize() - no regions found");
  }
}

DepList RegionList::GetDependencies() {
  return DepList(&OutStreamModule::GetGlobal());
}

RegionList::RegionList() : regionCount(0) {
}

MemoryRegion * RegionList::GetRegions() {
  return regions;
}

int RegionList::GetRegionCount() {
  return regionCount;
}

MemoryRegion * RegionList::FindRegion(uintptr_t ptr) {
  for (int i = 0; i < regionCount; i++) {
    if (regions[i].GetStart() > ptr) continue;
    if (regions[i].GetEnd() <= ptr) continue;
    return &regions[i];
  }
  return NULL;
}

MemoryRegion * RegionList::NextRegion(MemoryRegion * reg) {
  uintptr_t idx = ((uintptr_t)reg - (uintptr_t)regions)
    / sizeof(MemoryRegion);
  assert(idx < (uintptr_t)regionCount);
  if (idx + 1 == (uintptr_t)regionCount) return NULL;
  return reg + 1;
}

void RegionList::AddRegion(MemoryRegion & region) {
  if (regionCount == MaximumCount) {
    Panic("RegionList::AddRegion() - region overflow");
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

}

}
