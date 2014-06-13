#include <arch/x64/pmm/allocator.hpp>
#include <arch/x64/pmm/region-list.hpp>
#include <arch/x64/vmm/global-map.hpp>
#include <lock>
#include <panic>
#include <new>

namespace OS {

PhysicalAllocator & PhysicalAllocator::GetGlobal() {
  return x64::Allocator::GetGlobal();
}

namespace x64 {

static Allocator allocator;

void Allocator::Initialize(StepAllocator & alloc) {
  new(&allocator) Allocator(alloc);
}

Allocator & Allocator::GetGlobal() { 
  return allocator;
}

Allocator::Allocator(StepAllocator & alloc) {
  RegionList & regions = RegionList::GetGlobal();
  for (int i = 0; i < regions.GetRegionCount(); i++) {
    totalSpace += regions.GetRegions()[i].GetSize();
  }
  
  AllocatorList::InitInfo info(0x1000000, 0x1000, 0x1000,
                               regions.GetRegions(),
                               regions.GetRegionCount());
  allocators.SetInfo(info);
  allocators.GenerateDescriptions(true); // `true` is needed to sort it

  VirtAddr newAddress = AllocateRaw(alloc, allocators.BitmapByteCount());
  allocators.GenerateAllocators((uint8_t *)newAddress);
  allocators.Reserve(alloc.GetLastAddress());
  
  GlobalMap::GetGlobal().allocator = this;
}

PhysAddr Allocator::AllocLower(size_t size, size_t align, size_t * realSize) {
  ScopeLock scope(&lock);
  uintptr_t addr;
  bool res = allocators.AllocAscending(size, align, addr, realSize,
                                       0xFFFFFFFF);
  if (!res) return 0;
  return (PhysAddr)addr;
}

PhysAddr Allocator::Alloc(size_t size, size_t align, size_t * realSize) {
  ScopeLock scope(&lock);
  uintptr_t addr;
  bool res = allocators.AllocDescending(size, align, addr, realSize);
  if (!res) return 0;
  return (PhysAddr)addr;
}

void Allocator::Free(PhysAddr address) {
  ScopeLock scope(&lock);
  allocators.FreePointer(address);
}

size_t Allocator::Used() {
  return totalSpace - Available();
}

size_t Allocator::Available() {
  ScopeLock scope(&lock);
  return allocators.GetAvailableSpace();
}

size_t Allocator::Total() {
  return totalSpace;
}

PhysAddr Allocator::AllocPage() {
  return Alloc(0x1000, 0x1000, NULL);
}

void Allocator::FreePage(PhysAddr p) {
  Free(p);
}

// for initialization only
VirtAddr Allocator::AllocateRaw(StepAllocator & alloc, size_t size) {
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

}

}
