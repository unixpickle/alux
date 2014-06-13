#include <arch/x64/vmm/user-map.hpp>
#include <arch/x64/vmm/scope-scratch.hpp>
#include <arch/x64/vmm/global-map.hpp>
#include <arch/x64/smp/invlpg.hpp>
#include <arch/general/physical-allocator.hpp>
#include <panic>
#include <critical>
#include <lock>
#include <cassert>

namespace OS {

namespace x64 {

UserMap::UserMap(Task * t)
  : table(0), freeList(SpaceStart, SpaceSize), task(t) {
  PhysAddr pml4 = PhysicalAllocator::GetGlobal().Alloc(0x1000, 0x1000, NULL);
  if (!pml4) Panic("UserMap::UserMap() - could not allocate PML4");
  
  {
    TypedScratch<uint64_t> scratch(pml4);
    scratch[0] = GlobalMap::GetGlobal().GetPDPT() | 3;
    for (int i = 1; i < 0x200; i++) {
      scratch[i] = 0;
    }
  }
  
  table.SetPML4(pml4);
}

UserMap::~UserMap() {
  FreeTable(table.GetPML4(), 0, 1);
}

PhysAddr UserMap::GetPML4() {
  return table.GetPML4();
}

void UserMap::Set() {
  __asm__ __volatile__("mov %%rax, %%cr3" : : "a" (GetPML4()));
}

int UserMap::GetPageSizeCount() {
  return GlobalMap::GetGlobal().GetPageSizeCount();
}

size_t UserMap::GetPageSize(int index) {
  return GlobalMap::GetGlobal().GetPageSize(index);
}

size_t UserMap::GetPageAlignment(int index) {
  return GlobalMap::GetGlobal().GetPageAlignment(index);
}

bool UserMap::SupportsNX() {
  return true;
}

bool UserMap::SupportsRO() {
  return true;
}

bool UserMap::SupportsRemap() {
  return true;
}

void UserMap::Unmap(VirtAddr virt, UserMap::Size size) {
  AssertNoncritical();
  assert(virt >= SpaceStart);
  
  for (size_t i = 0; i < size.pageCount; i++) {
    ScopeLock scope(&lock);
    VirtAddr theAddr = virt + (size.pageSize * i);
    if (!table.Unset(theAddr)) {
      Panic("UserMap::Unmap() - table.Unset() failed");
    }
  }
  {
    ScopeLock scope(&lock);
    freeList.Free(virt, size.pageSize, size.pageCount);
  }
  DoInvlpg(virt, size.Total());
}

VirtAddr UserMap::Map(UserMap::MapInfo info) {
  AssertNoncritical();
  
  VirtAddr addr;
  {
    ScopeLock scope(&lock);
    addr = freeList.Alloc(info.pageSize, info.pageCount);
  }
  if (!addr) return 0;
  assert(addr >= SpaceStart);
  
  uint64_t mask = PageTable::EntryMask(info.pageSize, info.executable, false);
  if (!info.writable) mask ^= 2;
  uint64_t source = info.physical | mask;
  SetEntries(addr, source, info.pageSize, info.pageSize, info.pageCount);
  return addr;
}

void UserMap::MapAt(VirtAddr virt, UserMap::MapInfo info) {
  assert(virt >= SpaceStart);
  AssertNoncritical();
  
  uint64_t mask = PageTable::EntryMask(info.pageSize, info.executable, false);
  if (!info.writable) mask ^= 2;
  uint64_t source = info.physical | mask;
  SetEntries(virt, source, info.pageSize, info.pageSize, info.pageCount);
  DoInvlpg(virt, info.Total());
}

VirtAddr UserMap::Reserve(UserMap::Size size) {
  AssertNoncritical();
  ScopeLock scope(&lock);
  return freeList.Alloc(size.pageSize, size.pageCount);
}

void UserMap::FreeTable(PhysAddr table, int depth, int start) {
  AssertCritical();
  
  if (depth == 3) {
    SetCritical(false);
    PhysicalAllocator::GetGlobal().Free(table);
    SetCritical(true);
    return;
  }
  
  {
    TypedScratch<uint64_t> scratch(table);
    for (int i = start; i < 0x200; i++) {
      uint64_t entry = scratch[i];
      if (entry & 1) {
        PhysAddr addr = entry & 0x7FFFFFFFFFFFF000L;
        if (!(entry & 0x80)) { // don't free large pages!
          FreeTable(addr, depth + 1);
        }
        scratch.InvalidateCache();
      }
    }
  }
  
  SetCritical(false);
  PhysicalAllocator::GetGlobal().Free(table);
  SetCritical(true);
}

void UserMap::DoInvlpg(VirtAddr virt, size_t size) {
  DistributeUserInvlpg(virt, size, task);
}

void UserMap::SetEntries(VirtAddr virt, uint64_t phys, size_t virtAdd,
                         size_t physAdd, size_t count) {
  int depth = PageTable::PageSizeDepth(virtAdd);
  VirtAddr curVirt = virt;
  uint64_t curPhys = phys;

  for (size_t i = 0; i < count; i++) {
    ScopeLock scope(&lock);
    bool result = table.Set(curVirt, curPhys, 7, depth);
    if (!result) {
      Panic("UserMap::SetEntries() - table.Set() failed");
    }
    curPhys += physAdd;
    curVirt += virtAdd;
  }
}

}

}
