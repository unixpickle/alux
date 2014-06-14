#ifndef __X64_REGION_LIST_HPP__
#define __X64_REGION_LIST_HPP__

#include <module/module.hpp>
#include <analloc2>

namespace OS {

namespace x64 {

typedef ANAlloc::Region MemoryRegion;

class RegionList : public Module {
public:
  static const int MaximumCount = 8;  
  
  static void InitGlobal(void * mbootPtr);
  static RegionList & GetGlobal();
  
  virtual void Initialize();
  virtual Module ** GetDependencies(size_t & count);
  
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
