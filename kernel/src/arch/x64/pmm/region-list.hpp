#ifndef __X64_REGION_LIST_HPP__
#define __X64_REGION_LIST_HPP__

#include <module/module.hpp>
#include <arch-specific/types.hpp>
#include <analloc2>

namespace OS {

namespace x64 {

class RegionList : public Module {
public:
  static const int MaximumCount = 8;  
  
  static void InitGlobal(void * mbootPtr);
  static RegionList & GetGlobal();
  
  const ANAlloc::RegionList & GetLowerRegions();
  const ANAlloc::RegionList & GetUpperRegions();
  
  const ANAlloc::Region * FindRegion(PhysAddr addr);
  const ANAlloc::Region * NextRegion(const ANAlloc::Region * reg);
  
protected:
  ANAlloc::FixedRegionList<MaximumCount> lowerRegions;
  ANAlloc::FixedRegionList<MaximumCount> upperRegions;
  
  virtual void Initialize();
  virtual DepList GetDependencies();
  
  void AddRegion(const ANAlloc::Region & region);
};

}

}

#endif
