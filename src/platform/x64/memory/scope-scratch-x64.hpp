#ifndef __PLATFORM_X64_SCOPE_SCRATCH_X64_H__
#define __PLATFORM_X64_SCOPE_SCRATCH_X64_H__

#include <platform/memory.hpp>
#include <utilities/critical.hpp>

namespace OS {

namespace x64 {

class ScopeScratch : public ScopeCritical {
private:
  VirtAddr addr;

public:
  ScopeScratch(PhysAddr address);
  ~ScopeScratch();
  
  void * GetPointer();
  VirtAddr GetVirtAddr();
  void Reassign(PhysAddr newAddr);
  
};

template <class T>
class TypedScratch : public ScopeScratch {
public:
  TypedScratch(PhysAddr address)
    : ScopeScratch(address) {}
  
  T * GetTypedPointer() {
    return (T *)GetPointer();
  }
  
  T & operator[](int idx) {
    return GetTypedPointer()[idx];
  }
};

}

}

#endif
