#include <arch/x64/vmm/global-map.hpp>
#include <arch/x64/vmm/map-setup.hpp>
#include <arch/x64/vmm/scratch.hpp>
#include <arch/x64/smp/invlpg.hpp>
#include <arch/general/failure.hpp>
#include <utilities/lock.hpp>
#include <cstddef>
#include <cassert>
#include <new>

namespace OS {

AddressSpace & AddressSpace::GetGlobal() {
  return x64::GlobalMap::GetGlobal();
}

namespace x64 {

static GlobalMap instance;

void GlobalMap::Initialize(PageAllocator * anAlloc) {
  new(&instance) GlobalMap();
  instance.allocator = anAlloc;
  instance.Setup();
}

GlobalMap & GlobalMap::GetGlobal() {
  return instance;
}

GlobalMap::GlobalMap() : table(0), allocator(NULL), pdpt(0) {
}

PhysAddr GlobalMap::GetPDPT() {
  return pdpt;
}

PhysAddr GlobalMap::GetPML4() {
  return table.GetPML4();
}

void GlobalMap::Set() {
  __asm__("mov %0, %%cr3" : : "r" (table.GetPML4()) : "memory");
}

int GlobalMap::GetPageSizeCount() {
  return 2;
}

size_t GlobalMap::GetPageSize(int index) {
  if (index) return 0x200000;
  return 0x1000;
}

size_t GlobalMap::GetPageAlignment(int index) {
  return GetPageSize(index);
}

void GlobalMap::Unmap(VirtAddr virt, size_t pageSize, size_t pageCount) {
  {
    ScopeLock mainScope(&allocationLock);
    for (size_t i = 0; i < pageCount; i++) {
      ScopeLock scope(&tableLock);
      VirtAddr theAddr = virt + (pageSize * i);
      if (!table.Unset(theAddr)) {
        Panic("GlobalMap::Unmap() - table.Unset() failed");
      }
    }
    FreeRegion(virt, pageSize, pageCount);
  }
  // invalidate the memory
  DistributeKernelInvlpg(virt, pageSize * pageCount);
}

VirtAddr GlobalMap::Map(PhysAddr phys, size_t pageSize, size_t pageCount,
                        bool executable) {
  ScopeLock mainScope(&allocationLock);
  
  VirtAddr region = AllocateRegion(pageSize, pageCount);
  uint64_t source = phys | TableEntryMask(pageSize, executable);
  
  SetEntries(region, source, pageSize, pageSize, pageCount);
  return region;
}

void GlobalMap::MapAt(VirtAddr virt, PhysAddr phys, size_t pageSize,
                      size_t pageCount, bool executable) {
  uint64_t source = phys | TableEntryMask(pageSize, executable);
  SetEntries(virt, source, pageSize, pageSize, pageCount);
  
  // we may have overwritten something
  DistributeKernelInvlpg(virt, pageCount * pageSize);
}

VirtAddr GlobalMap::Reserve(size_t pageSize, size_t pageCount) {
  ScopeLock mainScope(&allocationLock);
  VirtAddr region = AllocateRegion(pageSize, pageCount);
  uint64_t entry = pageSize | (pageSize == 0x1000 ? 0 : 0x80);
  SetEntries(region, entry, pageSize, 0, pageCount);
  return region;
}

int GlobalMap::PageSizeDepth(size_t size) {
  switch (size) {
    case 0x1000:
      return 3;
    case 0x200000:
      return 2;
    case 0x40000000:
      return 1;
    default:
      break;
  }
  Panic("unknown page size");
  return -1;
}

uint64_t GlobalMap::TableEntryMask(size_t pageSize, bool exec) {
  return 0x103 | (pageSize == 0x1000 ? 0 : 0x80)
    | (exec ? 0 : (uint64_t)1 << 63);
}

void GlobalMap::Setup() {
  MapSetup setup(*allocator);
  setup.Map();
  
  table.SetPML4(setup.GetPML4());
  pdpt = setup.GetPDPT();
}

void GlobalMap::SetEntries(VirtAddr virt, PhysAddr phys, size_t virtAdd,
                           size_t physAdd, size_t count) {
  int depth = PageSizeDepth(virtAdd);
  VirtAddr curVirt = virt;
  PhysAddr curPhys = phys;

  for (size_t i = 0; i < count; i++) {
    ScopeLock scope(&tableLock);
    bool result = table.Set(curVirt, curPhys, 3, depth);
    if (!result) {
      Panic("GlobalMap::SetEntries() - table.Set() failed");
    }
    curPhys += physAdd;
    curVirt += virtAdd;
  }
}

VirtAddr GlobalMap::AllocateRegion(size_t pageSize, size_t pageCount) {
  if (!HasEnoughFree(pageSize, pageCount)) {
    UpdateFreeRegion();
    if (!HasEnoughFree(pageSize, pageCount)) {
      Panic("GlobalMap::AllocateRegion() - no contiguous regions available");
    }
  }
  
  if (freeStart % pageSize) {
    freeSize -= pageSize - freeStart % pageSize;
    freeStart += pageSize - freeStart % pageSize;
  }
  VirtAddr result = freeStart;
  freeSize -= pageSize * pageCount;
  freeStart += pageSize * pageCount;
  return result;
}

void GlobalMap::FreeRegion(VirtAddr addr, size_t pageSize, size_t pageCount) {
  size_t dataSize = pageSize * pageCount;
  if (addr == freeStart + freeSize) {
    // the chunk is right after the free region
    freeSize += dataSize;
  } else if (addr + pageSize * pageCount == freeStart) {
    // the chunk is right before the free region
    freeSize += dataSize;
    freeStart -= dataSize;
  }
}

void GlobalMap::UpdateFreeRegion() {
  VirtAddr addr = 0;
  while (addr < Scratch::StartAddr) {
    VirtAddr nextStart = addr;
    size_t nextSize = 0;
    
    while (1) {
      ScopeLock scope(&tableLock);
      size_t pageSize;
      uint64_t entry = 0;
      int result = table.Walk(nextStart + nextSize, entry, &pageSize);
      addr += pageSize;
      if (result >= 0 && entry) break;
      nextSize += pageSize;
      if (nextStart + nextSize >= Scratch::StartAddr) {
        nextSize = Scratch::StartAddr - nextStart;
        break;
      }
    }
    
    if (nextSize > freeSize) {
      freeSize = nextSize;
      freeStart = nextStart;
    }
  }
}

bool GlobalMap::HasEnoughFree(size_t pageSize, size_t pageCount) {
  if (!pageSize || !pageCount) return true;
  
  // when we allocate a page of size pageSize, it must be *aligned* to that
  // size.
  VirtAddr start = freeStart;
  size_t size = freeSize;
  if (start % pageSize) {
    // force alignment
    size_t adding = pageSize - start % pageSize;
    if (adding >= size) return false;
    start += adding;
    size -= adding;
  }
  return size >= (pageSize * pageCount);
}

}

}
