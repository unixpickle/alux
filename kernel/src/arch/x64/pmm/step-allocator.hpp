#ifndef __X64_STEP_ALLOCATOR_HPP__
#define __X64_STEP_ALLOCATOR_HPP__

#include <arch/x64/pmm/page-allocator.hpp>
#include <cstddef>

namespace OS {

namespace x64 {

class StepAllocator : public PageAllocator {
private:
  PhysAddr lastAddr;
  
public:
  StepAllocator(PhysAddr start);
  
  virtual PhysAddr AllocPage();
  virtual PhysAddr AllocSize(size_t size);
  virtual void FreePage(PhysAddr p);
  PhysAddr GetLastAddress();
};

}

}

#endif
