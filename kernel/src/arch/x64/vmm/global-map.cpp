#include <arch/x64/vmm/global-map.hpp>
#include <arch/x64/vmm/map-setup.hpp>
#include <arch/x64/vmm/scratch.hpp>
#include <arch/x64/vmm/kernel-layout.hpp>
#include <arch/x64/vmm/tlb.hpp>
#include <arch/x64/pmm/region-list.hpp>
#include <arch/x64/pmm/step-allocator.hpp>
#include <arch/general/physical-allocator.hpp>
#include <critical>
#include <iostream>
#include <cassert>
#include <cstddef>
#include <panic>
#include <lock>
#include <new>

namespace OS {

GlobalMap & GlobalMap::GetGlobal() {
  return x64::GlobalMap::GetGlobal();
}

namespace x64 {

static GlobalMap instance;
static StepAllocator step(0);

void GlobalMap::InitGlobal() {
  new(&instance) GlobalMap();
}

GlobalMap & GlobalMap::GetGlobal() {
  return instance;
}

GlobalMap::GlobalMap() : table(0) {
}

void GlobalMap::Initialize() {
  cout << "Initializing global map..." << endl;
  new(&step) StepAllocator(KernelSize());
  allocator = &step;
  Setup();
}

DepList GlobalMap::GetDependencies() {
  return DepList(&RegionList::GetGlobal(), &OutStreamModule::GetGlobal());
}

DepList GlobalMap::GetSuperDependencies() {
  return DepList(&PhysicalAllocator::GetGlobal(), &Scratch::GetGlobal());
}

PhysAddr GlobalMap::GetPDPT() {
  return pdpt;
}

PhysAddr GlobalMap::GetPML4() {
  return table.GetPML4();
}

void GlobalMap::Set() {
  AssertCritical();
  
  TLB::GetGlobal().WillSetUserMap(NULL);
  __asm__("mov %0, %%cr3" : : "r" (table.GetPML4()) : "memory");
}

bool GlobalMap::SupportsNX() {
  return true;
}

bool GlobalMap::SupportsRO() {
  return false;
}

bool GlobalMap::SupportsRemap() {
  return true;
}

void GlobalMap::Unmap(VirtAddr virt, GlobalMap::Size size) {
  ScopeLock scope(&lock);
  for (size_t i = 0; i < size.pageCount; i++) {
    VirtAddr theAddr = virt + (size.pageSize * i);
    if (!table.Unset(theAddr)) {
      Panic("GlobalMap::Unmap() - table.Unset() failed");
    }
  }
  FreeRegion(virt, size);
  
  // invalidate the memory
  TLB::GetGlobal().InvlpgGlobal(virt, size.Total());
}

VirtAddr GlobalMap::Map(GlobalMap::MapInfo info) {
  ScopeLock scope(&lock);
  
  VirtAddr region = AllocateRegion(info);
  uint64_t mask = PageTable::EntryMask(info.pageSize, info.executable, true);
  uint64_t source = info.physical | mask;
  
  SetEntries(region, source, info.pageSize, info.pageSize, info.pageCount);
  return region;
}

void GlobalMap::MapAt(VirtAddr virt, GlobalMap::MapInfo info) {
  ScopeLock scope(&lock);
  
  uint64_t mask = PageTable::EntryMask(info.pageSize, info.executable, true);
  uint64_t source = info.physical | mask;
  SetEntries(virt, source, info.pageSize, info.pageSize, info.pageCount);
  
  // we may have overwritten something
  TLB::GetGlobal().InvlpgGlobal(virt, info.Total());
}

VirtAddr GlobalMap::Reserve(GlobalMap::Size size) {
  ScopeLock scope(&lock);
  
  VirtAddr region = AllocateRegion(size);
  uint64_t entry = size.pageSize | (size.pageSize == 0x1000 ? 0 : 0x80);
  SetEntries(region, entry, size.pageSize, 0, size.pageCount);
  
  return region;
}

void GlobalMap::Setup() {
  MapSetup setup(*allocator);
  setup.Map();
  
  table.SetPML4(setup.GetPML4());
  pdpt = setup.GetPDPT();
}

void GlobalMap::SetEntries(VirtAddr virt, uint64_t phys, size_t virtAdd,
                           size_t physAdd, size_t count) {
  int depth = PageTable::PageSizeDepth(virtAdd);
  VirtAddr curVirt = virt;
  PhysAddr curPhys = phys;

  for (size_t i = 0; i < count; i++) {
    bool result = table.Set(curVirt, curPhys, 3, depth);
    if (!result) {
      Panic("GlobalMap::SetEntries() - table.Set() failed");
    }
    curPhys += physAdd;
    curVirt += virtAdd;
  }
}

VirtAddr GlobalMap::AllocateRegion(GlobalMap::Size size) {
  if (!HasEnoughFree(size)) {
    UpdateFreeRegion();
    if (!HasEnoughFree(size)) {
      Panic("GlobalMap::AllocateRegion() - no contiguous regions available");
    }
  }
  
  if (freeStart % size.pageSize) {
    freeSize -= size.pageSize - freeStart % size.pageSize;
    freeStart += size.pageSize - freeStart % size.pageSize;
  }
  VirtAddr result = freeStart;
  freeSize -= size.Total();
  freeStart += size.Total();
  return result;
}

void GlobalMap::FreeRegion(VirtAddr addr, GlobalMap::Size size) {
  size_t dataSize = size.Total();
  if (addr == freeStart + freeSize) {
    // the chunk is right after the free region
    freeSize += dataSize;
  } else if (addr + dataSize == freeStart) {
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

bool GlobalMap::HasEnoughFree(GlobalMap::Size size) {
  if (!size.pageSize || !size.pageCount) return true;
  
  // when we allocate a page, it must be aligned to pageSize
  size_t available = freeSize;
  if (freeStart % size.pageSize) {
    // force alignment
    size_t padding = size.pageSize - freeStart % size.pageSize;
    if (padding >= available) return false;
    available -= padding;
  }
  return size.Total() <= available;
}

}

}
