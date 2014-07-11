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

const ANAlloc::RegionList & RegionList::GetLowerRegions() {
  return lowerRegions;
}

const ANAlloc::RegionList & RegionList::GetUpperRegions() {
  return upperRegions;
}

const ANAlloc::Region * RegionList::FindRegion(PhysAddr addr) {
  for (int i = 0; i < lowerRegions.GetCount(); i++) {
    if (lowerRegions[i].Contains((ANAlloc::UInt)addr)) {
      return &lowerRegions[i];
    }
  }
  for (int i = 0; i < upperRegions.GetCount(); i++) {
    if (upperRegions[i].Contains((ANAlloc::UInt)addr)) {
      return &upperRegions[i];
    }
  }
  return NULL;
}

const ANAlloc::Region * RegionList::NextRegion(const ANAlloc::Region * reg) {
  for (int i = 0; i < lowerRegions.GetCount(); i++) {
    if (lowerRegions[i].GetStart() > reg->GetStart()) {
      return &lowerRegions[i];
    }
  }
  for (int i = 0; i < upperRegions.GetCount(); i++) {
    if (upperRegions[i].GetStart() > reg->GetStart()) {
      return &upperRegions[i];
    }
  }
  return NULL;
}

// PROTECTED //

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
    ANAlloc::UInt start = (ANAlloc::UInt)info->base_addr;
    ANAlloc::UInt len = (ANAlloc::UInt)info->length;
    
    // if this region is in the lower MB, we will stretch it out to the
    // beginning of the address space.
    if (start <= 0x100000) {
      len += start;
      start = 0;
    }
    
    ANAlloc::Region region(start, len);
    if (start < 0x100000000L && start + len > 0x100000000L) {
      // this will probably never happen, but if it does we need to split the
      // regions for ANAlloc to be able to differentiate between upper and
      // lower memory
      ANAlloc::UInt lowSize = (ANAlloc::UInt)(0x100000000L - start);
      ANAlloc::Region lowerRegion(start, lowSize);
      ANAlloc::Region upperRegion(0x100000000L, len - lowSize);
      AddRegion(lowerRegion);
      AddRegion(upperRegion);
    } else {
      AddRegion(region);
    }
  }
  if (!lowerRegions.GetCount()) {
    Panic("RegionList::Initialize() - no lower regions found");
  }
}

DepList RegionList::GetDependencies() {
  return DepList(&OutStreamModule::GetGlobal());
}

void RegionList::AddRegion(const ANAlloc::Region & region) {
  ANAlloc::FixedRegionList<MaximumCount> * list = NULL;
  
  if (region.GetStart() < 0x100000000L) {
    list = &lowerRegions;
  } else {
    list = &upperRegions;
  }
  
  int insertIndex = list->GetCount();
  for (int i = 0; i < list->GetCount(); i++) {
    if ((*list)[i].GetStart() > region.GetStart()) {
      insertIndex = i;
      break;
    }
  }
  if (!list->Insert(region, insertIndex)) {
    Panic("RegionList::AddRegion() - region overflow");
  }
}

}

}
