#ifndef __PLATFORM_X64_STEP_ALLOCATOR_X64_H__
#define __PLATFORM_X64_STEP_ALLOCATOR_X64_H__

#include "page-allocator-x64.hpp"
#include "phys-region-list-x64.hpp"

namespace OS {

namespace x64 {

class StepAllocator : public PageAllocator {
private:
  PhysRegionList * regions;

protected:
  PhysAddr lastAddr;
  
public:
  StepAllocator(PhysRegionList * _regions, PhysAddr addr);
  
  virtual PhysAddr AllocPage();
  virtual PhysAddr AllocSize(size_t size);
  virtual void FreePage(PhysAddr addr);

  friend class PMM;
};

}

}

#endif
