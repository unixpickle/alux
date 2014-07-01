#ifndef __X64_ALLOCATOR_HPP__
#define __X64_ALLOCATOR_HPP__

#include <arch/x64/pmm/step-allocator.hpp>
#include <arch/general/physical-allocator.hpp>
#include <module/module.hpp>
#include <analloc2>
#include <panic>
#include <macros>

namespace OS {

namespace x64 {

class Allocator : public PhysicalAllocator, public PageAllocator {
public:
  static const int MaximumAllocators = 0x10;
  typedef ANAlloc::BBTree TreeType;
  typedef ANAlloc::AllocatorList<MaximumAllocators, TreeType> AllocatorList;
  
  static void InitGlobal();
  static Allocator & GetGlobal();
  
  PhysAddr AllocLower(size_t size, size_t align, size_t * realSize);
  
  virtual PhysAddr Alloc(size_t size, size_t align, size_t * realSize);
  virtual void Free(PhysAddr address);
  virtual size_t Used();
  virtual size_t Available();
  virtual size_t Total();
  
  virtual PhysAddr AllocPage();
  virtual void FreePage(PhysAddr p);
  
protected:
  virtual DepList GetDependencies();
  virtual void Initialize();
  
private:
  AllocatorList allocators;
  uint64_t lock OS_ALIGNED(8);
  size_t totalSpace;
  
  static VirtAddr AllocateRaw(StepAllocator & alloc, size_t size);
  
};

}

}

#endif
