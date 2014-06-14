#include <scheduler/user/code-map.hpp>
#include <scheduler/general/hold-scope.hpp>
#include <arch/general/physical-allocator.hpp>
#include <memory/easy-map.hpp>
#include <critical>
#include <cstring>
#include <lock>

namespace OS {

CodeMap::CodeMap(Task * t, UserCode * c) : task(t), code(c) {
  AssertNoncritical();
  AddressSpace::Size size = code->GetPhysSize();
  assert(size.pageSize >= code->GetPageAlignment());
  
  code->Retain();
  
  AddressSpace & space = t->GetAddressSpace();
  if (space.SupportsRemap()) {
    pages = new PageStatus[size.pageCount];
    spaceStart = space.Reserve(size);
  } else {
    PhysicalAllocator & allocator = PhysicalAllocator::GetGlobal();
    allocated = allocator.Alloc(size.Total(), code->GetPageAlignment(), NULL);
    assert(allocated != 0);
    AddressSpace::MapInfo info(size.pageSize, size.pageCount, allocated);
    spaceStart = space.Map(info);
    assert(spaceStart != 0);
  }
}

CodeMap::~CodeMap() {
  AssertNoncritical();
  if (pages) {
    for (size_t i = 0; i < code->GetPhysSize().pageCount; i++) {
      if (pages[i].isCopied) {
        PhysicalAllocator::GetGlobal().Free(pages[i].phys);
      }
    }
    delete pages;
  } else {
    PhysicalAllocator::GetGlobal().Free(allocated);
  }
  code->Release();
}

bool CodeMap::HandleFault(VirtAddr addr, bool, bool write) {
  AssertNoncritical();
  HoldScope hold(task);
  if (!hold.DidHold()) return false;
  ScopeLock scope(&lock);
  
  if (!pages) return false; // no remap
  
  if (addr < spaceStart) return false;
  if (addr >= spaceStart + code->GetPhysSize().Total()) return false;
  
  size_t pageSize = code->GetPhysSize().pageSize;
  size_t index = (addr - spaceStart) / pageSize;
  PageStatus & status = pages[index];
  if (status.isCopied || (status.isMapped && !write)) {
    // we faulted with an okay page, tell them to try again
    return true;
  }
  
  VirtAddr location = spaceStart + (index * pageSize);
  PhysAddr source = code->GetPhysStart() + (index * pageSize);
  if (write || !task->GetAddressSpace().SupportsRO()) {
    // copy the thing
    PhysicalAllocator & allocator = PhysicalAllocator::GetGlobal();
    PhysAddr addr = allocator.Alloc(pageSize, code->GetPageAlignment(), NULL);
    assert(addr != 0);
    
    // copy the original page into the newly allocated one
    {
      EasyMap dest(addr, pageSize);
      EasyMap sourceMap(source, pageSize);
      memcpy((void *)dest.GetStart(), (void *)sourceMap.GetStart(), pageSize);
    }
    
    status.isCopied = true;
    status.phys = addr;
    AddressSpace::MapInfo info(pageSize, 1, addr);
    task->GetAddressSpace().MapAt(location, info);
  } else {
    // map the thing
    status.isMapped = true;
    AddressSpace::MapInfo info(pageSize, 1, source, true, false);
    task->GetAddressSpace().MapAt(location, info);
  }
  
  return true;
}

VirtAddr CodeMap::GetStart() {
  return spaceStart;
}

}