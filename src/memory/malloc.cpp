#include <memory/malloc.hpp>
#include <arch/general/address-space.hpp>
#include <arch/general/physical-allocator.hpp>
#include <arch/general/failure.hpp>
#include <cassert>
#include <new>

namespace OS {

static Malloc globalMalloc;

void Malloc::Initialize() {
  new(&globalMalloc) Malloc();
}

Malloc & Malloc::GetGlobal() {
  return globalMalloc;
}

Malloc::Malloc() {
  // calculate the pageSize and pageAlignment to use
  AddressSpace & space = AddressSpace::GetGlobal();
  
  int sizeCount = space.GetPageSizeCount();
  assert(sizeCount > 0);
  
  int useSizeIndex = 0;
  pageSize = space.GetPageSize(0);
  
  for (int i = 1; i < sizeCount; i++) {
    size_t size = space.GetPageSize(i);
    if (size > RegionSize) break;
    pageSize = size;
    useSizeIndex = i;
  }
  
  pageAlignment = space.GetPageAlignment(useSizeIndex);
  
  allocSize = RegionSize;
  if (RegionSize % pageSize) {
    allocSize += pageSize - (RegionSize % pageSize);
  }
}

void * Malloc::Alloc(size_t size, bool getNew) {
  if (size > allocSize / 2) return 0;
  
  if (!getNew) {
    MallocRegion * reg = first;
    while (reg) {
      void * ptr = reg->AllocBuf(size);
      if (ptr) return ptr;
      reg = reg->next;
    }
    return 0;
  }
  
  void * addr = Alloc(size, false);
  if (addr) return addr;
  MakeNewRegion();
  return Alloc(size, false);
}

void Malloc::Free(void * addr) {
  MallocRegion * reg = first;
  while (reg) {
    if (reg->OwnsPointer(addr)) {
      reg->FreeBuf(addr);
      return;
    }
    reg = reg->next;
  }
  Panic("Free(): attempt to free unowned pointer");
}

void Malloc::MakeNewRegion() {
  PhysicalAllocator & physAlloc = PhysicalAllocator::GetGlobal();
  AddressSpace & space = AddressSpace::GetGlobal();
  PhysAddr addr = physAlloc.Alloc(allocSize, pageAlignment, NULL);
  VirtAddr vAddr = space.Map(addr, pageSize, allocSize / pageSize);
  
  void * region = (void *)vAddr;
  MallocRegion * last = first;
  first = new(region) MallocRegion(region, allocSize, sizeof(MallocRegion));
  first->next = last;
}
  
}
