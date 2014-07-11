#ifndef __X64_ALLOCATOR_HPP__
#define __X64_ALLOCATOR_HPP__

#include <arch/x64/pmm/step-allocator.hpp>
#include <arch/general/physical-allocator.hpp>
#include <panic>
#include <macros>
#include <analloc2>

namespace OS {

namespace x64 {

class Allocator : public PhysicalAllocator, public PageAllocator {
public:
  static const int MaxAllocators = 0x10;
  
  static void InitGlobal();
  static Allocator & GetGlobal();
  
  virtual PhysAddr AllocLower(size_t size, size_t align);
  
  // OS::PhysicalAllocator
  virtual PhysAddr Alloc(size_t size, size_t align);
  virtual void Free(PhysAddr address);
  virtual size_t Used();
  virtual size_t Available();
  virtual size_t Total();
  
  // OS::x64::PageAllocator
  virtual PhysAddr AllocPage();
  virtual void FreePage(PhysAddr p);
  
protected:
  virtual DepList GetDependencies();
  virtual void Initialize();
  
private:
  ANAlloc::FixedCluster<MaxAllocators> lower;
  ANAlloc::FixedCluster<MaxAllocators> upper;
  bool hasUpper = false;
  
  uint64_t upperLock OS_ALIGNED(8) = 0;
  uint64_t lowerLock OS_ALIGNED(8) = 0;
  
  size_t totalSpace = 0;
  
  static VirtAddr AllocateRaw(size_t size);
  void InitializeCluster(ANAlloc::MutableCluster & cluster,
                         const ANAlloc::RegionList & regs);  
};

}

}

#endif
