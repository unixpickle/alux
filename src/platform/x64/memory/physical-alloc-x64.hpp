#ifndef __PLATFORM_X64_PHYSICAL_ALLOC_X64_H__
#define __PLATFORM_X64_PHYSICAL_ALLOC_X64_H__

#include <platform/memory.hpp>
#include <analloc2>
#include "kernel-map-x64.hpp"
#include "step-allocator-x64.hpp"

namespace OS {

namespace x64 {

void InitializeKernAllocator(void * mbootPtr);
KernelMap * GetGlobalKernelMap();

class PhysicalAllocator : public PageAllocator {
public:
  static const int MaximumAllocators = 0x10;
  typedef ANAlloc::BBTree TreeType;
  typedef ANAlloc::AllocatorList<MaximumAllocators, TreeType> AllocatorList;
  
  static void Initialize(void * mbootPtr);
  static PhysicalAllocator & GetGlobal();
  
  PhysicalAllocator() {
    Panic("This is just for the nice compiler.");
  }
  
  PhysicalAllocator(void * mbootPtr);
  void Setup();
  
  bool Alloc(size_t size, PhysAddr & addr, size_t * realSize);
  bool Align(size_t size, size_t align, PhysAddr & addr, size_t * realSize);
  bool AllocBelow(size_t size, PhysAddr & addr, size_t * realSize,
                  uintptr_t boundary);
  bool AlignBelow(size_t size, size_t align, PhysAddr & addr,
                  size_t * realSize, uintptr_t boundary);
  void Free(PhysAddr addr);
  size_t Available();
  size_t Used();
  
  virtual PhysAddr AllocPage();
  virtual void FreePage(PhysAddr p);

private:
  PhysRegionList regions;
  AllocatorList allocators;
  uint64_t lock OS_ALIGNED(8);
  size_t totalSpace = 0;
  
  VirtAddr AllocateBitmaps(StepAllocator & alloc,
                           PhysAddr & firstFree);
  void GrabSpace(bool large,
                 StepAllocator & alloc,
                 bool & hasStarted,
                 uint64_t & lastPtr,
                 uint64_t & firstPtr);

};

}

}

#endif
