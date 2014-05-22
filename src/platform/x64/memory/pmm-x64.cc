#include "pmm-x64.hpp"

namespace OS {
namespace x64 {

static PMM allocator;

void PMM::Initialize(void * mbootPtr) {
  new(&allocator) PMM(mbootPtr);
  allocator.Setup();
}

PMM & PMM::GetGlobal() {
  return allocator;
}

PMM::PMM(void * mbootPtr)
  : regions(mbootPtr), lock(0), totalSpace(0) {
  for (int i = 0; i < regions.GetRegionCount(); i++) {
    totalSpace += regions.GetRegions()[i].GetSize();
  }
}

void PMM::Setup() {
  cout << "PMM::Setup() - regions:";
  for (int i = 0; i < regions.GetRegionCount(); i++) {
    cout << " [" << regions.GetRegions()[i].GetStart() << ":"
      << regions.GetRegions()[i].GetEnd() << "]";
  }
  cout << endl;
  
  KernelMap & kernMap = KernelMap::GetGlobal();
  StepAllocator stepper(&regions, KernelDataSize());
  kernMap.allocator = &stepper;
  kernMap.Setup();
  kernMap.Set();
  
  AllocatorList::InitInfo info(0x1000000, 0x1000, 0x1000,
                               regions.GetRegions(),
                               regions.GetRegionCount());
  allocators.SetInfo(info);
  allocators.GenerateDescriptions(true);
  
  // allocate enough physical memory for the bitmap data
  PhysAddr firstFree;
  VirtAddr bitmapPtr = AllocateBitmaps(stepper, firstFree);
  
  allocators.GenerateAllocators((uint8_t *)bitmapPtr);
  allocators.Reserve(firstFree);
  
  kernMap.allocator = this;
  
  cout << "OS::x64::PMM::Setup() - firstFree="
    << firstFree << endl;
}

bool PMM::Alloc(size_t size,
                              PhysAddr & addr,
                              size_t * realSize) {
  return Align(size, 1, addr, realSize);
}

bool PMM::Align(size_t size,
                size_t align,
                PhysAddr & addr,
                size_t * realSize) {
  ScopeLock scope(&lock);
  return allocators.AllocDescending(size, align, (uintptr_t &)addr,
                                    realSize);
}

bool PMM::AllocBelow(size_t size, PhysAddr & addr,
                     size_t * realSize, uintptr_t boundary) {
  return AlignBelow(size, 1, addr, realSize, boundary);
}

bool PMM::AlignBelow(size_t size, size_t align,
                     PhysAddr & addr, size_t * realSize,
                     uintptr_t boundary) {
  ScopeLock scope(&lock);
  return allocators.AllocAscending(size, align, (uintptr_t &)addr,
                                   realSize, boundary - 1);
}

void PMM::Free(PhysAddr addr) {
  ScopeLock scope(&lock);
  allocators.FreePointer(addr);
}

size_t PMM::Available() {
  ScopeLock scope(&lock);
  return allocators.GetAvailableSpace();
}

size_t PMM::Used() {
  return totalSpace - Available();
}

PhysAddr PMM::AllocPage() {
  PhysAddr result;
  bool stat = Align(0x1000, 0x1000, result, NULL);
  if (!stat) Panic("OS::x64::PMM::AllocPage() - failed");
  return result;
}

void PMM::FreePage(PhysAddr p) {
  Free(p);
}

/***********
 * Private *
 ***********/

void PMM::GrabSpace(bool large,
                    StepAllocator & alloc,
                    bool & hasStarted,
                    uint64_t & lastPtr,
                    uint64_t & firstPtr) {
  // allocate the data
  KernelMap & kernMap = KernelMap::GetGlobal();
  size_t pageSize = large ? 0x200000 : 0x1000;
  PhysAddr physPtr = alloc.AllocSize(pageSize);

  // map
  if (hasStarted) {
    kernMap.MapAt(lastPtr, physPtr, pageSize, large);
    lastPtr += pageSize;
  } else {
    hasStarted = true;
    firstPtr = kernMap.Map(physPtr, pageSize, large);
    if (!firstPtr) {
      Panic("OS::x64::PMM::GrabSpace() - Map() failed");
    }
    lastPtr = firstPtr + pageSize;
  }
}

VirtAddr PMM::AllocateBitmaps(StepAllocator & alloc,
                              PhysAddr & firstFree) {
  ssize_t remaining = (ssize_t)allocators.BitmapByteCount();
  
  // DEBUG: set this to make sure large pages work
  // remaining = 0x400000;
  
  bool hasStarted = false;
  uint64_t lastPtr;
  uint64_t firstPtr;
  size_t pageSize = (remaining >= 0x200000 ? 0x200000 : 0x1000);
  while (remaining > 0) {
    GrabSpace(pageSize == 0x200000, alloc, hasStarted, lastPtr, firstPtr);
    remaining -= pageSize;
  }
  firstFree = alloc.lastAddr;
  return firstPtr;
}

}
}
