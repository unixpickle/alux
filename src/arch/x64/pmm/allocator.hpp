#ifndef __X64_ALLOCATOR_HPP__
#define __X64_ALLOCATOR_HPP__

#include <arch/x64/pmm/step-allocator.hpp>

namespace OS {

namespace x64 {

class Allocator : public PageAllocator {
public:
  static void Initialize(StepAllocator & allocator);
  static Allocator & GetGlobal();
  
  virtual PhysAddr AllocPage();
  virtual void FreePage(PhysAddr p);
};

}

}

#endif
