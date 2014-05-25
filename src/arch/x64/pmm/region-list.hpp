#ifndef __X64_REGION_LIST_HPP__
#define __X64_REGION_LIST_HPP__

#include <analloc2>

namespace OS {

namespace x64 {

typedef ANAlloc::Region MemoryRegion;

class RegionList {
public:
  static const int MaximumCount = 8;  
  
  static void Initialize(void * mbootPtr);
  static RegionList & GetGlobal();
  
  RegionList();
  MemoryRegion * GetRegions();
  int GetRegionCount();
  MemoryRegion * FindRegion(uintptr_t ptr);
  MemoryRegion * NextRegion(MemoryRegion * reg);
  
protected:
  MemoryRegion regions[MaximumCount];
  int regionCount;
  
  void AddRegion(MemoryRegion & region);
};

}

}

#endif
