#ifndef __PLATFORM_X64_PHYS_REGION_LIST_X64_H__
#define __PLATFORM_X64_PHYS_REGION_LIST_X64_H__

#include "../multiboot-x64.hpp"
#include <platform/memory.hpp>
#include <analloc2>
#include <cassert>

namespace OS {

namespace x64 {

const int MaximumPhysicalRegions = 8;
typedef ANAlloc::Region MemoryRegion;

class PhysRegionList {
private:
  MemoryRegion regions[MaximumPhysicalRegions];
  int regionCount;
  
  void AddRegion(MemoryRegion & region);
  
public:
  PhysRegionList() {}
  PhysRegionList(void * mbootPtr);
  MemoryRegion * GetRegions();
  int GetRegionCount();
  MemoryRegion * FindRegion(uintptr_t ptr);
  MemoryRegion * NextRegion(MemoryRegion * reg);
  
};

}

}

#endif

