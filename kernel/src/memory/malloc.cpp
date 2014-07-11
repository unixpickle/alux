#include <memory/malloc.hpp>
#include <arch/general/global-map.hpp>
#include <arch/general/physical-allocator.hpp>
#include <cassert>
#include <new>
#include <panic>

namespace OS {

static Malloc globalMalloc;

void Malloc::InitGlobal() {
  new(&globalMalloc) Malloc();
}

Malloc & Malloc::GetGlobal() {
  return globalMalloc;
}

void Malloc::Initialize() {
  // calculate the pageSize and pageAlignment to use
  int sizeCount = AddressSpace::GetPageSizeCount();
  assert(sizeCount > 0);
  
  int useSizeIndex = 0;
  pageSize = AddressSpace::GetPageSize(0);
  
  for (int i = 1; i < sizeCount; i++) {
    size_t size = AddressSpace::GetPageSize(i);
    if (size > RegionSize) break;
    pageSize = size;
    useSizeIndex = i;
  }
  
  pageAlignment = AddressSpace::GetPageAlignment(useSizeIndex);
  
  allocSize = RegionSize;
  if (RegionSize % pageSize) {
    allocSize += pageSize - (RegionSize % pageSize);
  }
}

DepList Malloc::GetDependencies() {
  return DepList(&PhysicalAllocator::GetGlobal(), &GlobalMap::GetGlobal());
}

void * Malloc::Alloc(size_t size, bool getNew) {
  if (size > allocSize / 2) return 0;
  
  if (!getNew) {
    MallocLink * link = first;
    while (link) {
      void * ptr = link->malloc->Alloc(size);
      if (ptr) return ptr;
      link = link->next;
    }
    return 0;
  }
  
  void * addr = Alloc(size, false);
  if (addr) return addr;
  MakeNewRegion();
  return Alloc(size, false);
}

void Malloc::Free(void * addr) {
  MallocLink * link = first;
  while (link) {
    if (link->malloc->OwnsPointer(addr)) {
      link->malloc->Free(addr);
      return;
    }
    link = link->next;
  }
  Panic("Malloc::Free() - attempt to free unowned pointer");
}

void Malloc::MakeNewRegion() {
  PhysicalAllocator & physAlloc = PhysicalAllocator::GetGlobal();
  AddressSpace & space = GlobalMap::GetGlobal();
  PhysAddr addr = physAlloc.Alloc(allocSize, pageAlignment);
  
  AddressSpace::MapInfo info(pageSize, allocSize / pageSize, addr);
  VirtAddr vAddr = space.Map(info);
  
  assert(vAddr > 0);
  
  uint8_t * region = (uint8_t *)vAddr;
  MallocLink * link = MallocLink::Wrap(region, allocSize);
  link->next = first;
  first = link;
}
  
}
