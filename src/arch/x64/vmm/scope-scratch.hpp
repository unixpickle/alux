#ifndef __X64_SCOPE_SCRATCH_HPP__
#define __X64_SCOPE_SCRATCH_HPP__

#include <utilities/critical.hpp>
#include <specific/types.hpp>

namespace OS {

namespace x64 {

class ScopeScratch : public ScopeCritical {
private:
  VirtAddr ptr;

public:
  /**
   * @ambicritical (x) -> @critical
   */
  ScopeScratch(PhysAddr address);
  
  /**
   * @critical -> @ambicritical (x)
   */
  ~ScopeScratch();
  
  /**
   * @critical
   */
  void * GetPointer();
  
  /**
   * @critical
   */
  VirtAddr GetVirtAddr();
  
  /**
   * @critical
   */
  void Reassign(PhysAddr newAddr);
};

template <class T>
class TypedScratch : public ScopeScratch {
public:
  /**
   * @ambicritical -> @critical
   */
  TypedScratch(PhysAddr address)
    : ScopeScratch(address) {}
  
  /**
   * @critical
   */
  T * GetTypedPointer() {
    return (T *)GetPointer();
  }
  
  /**
   * @critical
   */
  T & operator[](int idx) {
    return GetTypedPointer()[idx];
  }
};

}

}

#endif