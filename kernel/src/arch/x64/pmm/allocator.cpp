#include <arch/x64/pmm/allocator.hpp>
#include <arch/x64/pmm/region-list.hpp>
#include <arch/x64/vmm/global-map.hpp>
#include <arch/x64/vmm/scratch.hpp>
#include <iostream>
#include <panic>
#include <lock>
#include <new>

namespace OS {

PhysicalAllocator & PhysicalAllocator::GetGlobal() {
  return x64::Allocator::GetGlobal();
}

namespace x64 {

static Allocator allocator;

void Allocator::InitGlobal() {
  new(&allocator) Allocator();
}

Allocator & Allocator::GetGlobal() { 
  return allocator;
}

PhysAddr Allocator::AllocLower(size_t size, size_t align) {
  ScopeLock scope(&lowerLock);
  ANAlloc::UInt addr;
  bool res = lower.Align((ANAlloc::UInt)size, (ANAlloc::UInt)align, addr);
  if (!res) return 0;
  return (PhysAddr)addr;
}

PhysAddr Allocator::Alloc(size_t size, size_t align) {
  if (!hasUpper) {
    return AllocLower(size, align);
  }
  {
    ScopeLock scope(&upperLock);
    ANAlloc::UInt addr;
    bool res = upper.Align((ANAlloc::UInt)size, (ANAlloc::UInt)align, addr);
    if (res) return (PhysAddr)addr;
  }
  return AllocLower(size, align);
}

void Allocator::Free(PhysAddr address) {
  if (address < 0x100000000L) {
    ScopeLock scope(&lowerLock);
    lower.Free((ANAlloc::UInt)address);
  } else {
    ScopeLock scope(&upperLock);
    upper.Free((ANAlloc::UInt)address);
  }
}

size_t Allocator::Used() {
  return totalSpace - Available();
}

size_t Allocator::Available() {
  size_t available = 0;
  {
    ScopeLock scope(&lowerLock);
    available += (size_t)lower.GetFreeSize();
  }
  {
    ScopeLock scope(&upperLock);
    available += (size_t)upper.GetFreeSize();
  }
  return available;
}

size_t Allocator::Total() {
  return totalSpace;
}

PhysAddr Allocator::AllocPage() {
  return Alloc(0x1000, 0x1000);
}

void Allocator::FreePage(PhysAddr p) {
  Free(p);
}

// PROTECTED //

DepList Allocator::GetDependencies() {
  return DepList(&GlobalMap::GetGlobal(), &Scratch::GetGlobal(),
                 &RegionList::GetGlobal(), &OutStreamModule::GetGlobal());
}

void Allocator::Initialize() {
  cout << "Initializing physical allocator..." << endl;
  
  RegionList & regions = RegionList::GetGlobal();
  for (int i = 0; i < regions.GetLowerRegions().GetCount(); i++) {
    totalSpace += (size_t)regions.GetLowerRegions()[i].GetSize();
  }
  for (int i = 0; i < regions.GetUpperRegions().GetCount(); i++) {
    totalSpace += (size_t)regions.GetUpperRegions()[i].GetSize();
  }
  
  InitializeCluster(lower, regions.GetLowerRegions());
  if (regions.GetUpperRegions().GetCount()) {
    InitializeCluster(upper, regions.GetUpperRegions());
    hasUpper = true;
  }
  
  PageAllocator & theAllocator = *GlobalMap::GetGlobal().allocator;
  StepAllocator & alloc = static_cast<StepAllocator &>(theAllocator);
  
  lower.Reserve(0, (ANAlloc::UInt)alloc.GetLastAddress());
  if (hasUpper) {
    upper.Reserve(0, (ANAlloc::UInt)alloc.GetLastAddress());
  }

  GlobalMap::GetGlobal().allocator = this;
}

// PRIVATE //

VirtAddr Allocator::AllocateRaw(size_t size) {
  PageAllocator & theAllocator = *GlobalMap::GetGlobal().allocator;
  StepAllocator & alloc = static_cast<StepAllocator &>(theAllocator);
  
  GlobalMap & map = GlobalMap::GetGlobal();
  
  size_t pageSize = size >= 0x200000 ? 0x200000 : 0x1000;
  size_t pageCount = size / pageSize + (size % pageSize ? 1 : 0);
  VirtAddr reserved = map.Reserve(GlobalMap::Size(pageSize, pageCount));
  VirtAddr dest = reserved;
  
  while (pageCount--) {
    PhysAddr page = alloc.AllocSize(pageSize);
    map.MapAt(dest, GlobalMap::MapInfo(pageSize, 1, page, false));
    dest += pageSize;
  }
  
  return reserved;
}

void Allocator::InitializeCluster(ANAlloc::MutableCluster & cluster,
                                  const ANAlloc::RegionList & regs) {
  ANAlloc::FixedDescList<MaxAllocators> descs;
  
  // create the descriptions
  ANAlloc::Layout layout(descs, regs, 12, 0x1000000, 0x1000);
  layout.Run();
  
  // create the allocators and their trees
  ANAlloc::ClusterBuilder<ANAlloc::BBTree> builder(descs, cluster, 12);
  ANAlloc::UInt space = builder.RequiredSpace();
  VirtAddr newAddress = AllocateRaw((size_t)space);
  builder.CreateAllocators((uint8_t *)newAddress);
}

}

}
