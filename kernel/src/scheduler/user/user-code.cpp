#include <scheduler/user/user-code.hpp>
#include <arch/general/physical-allocator.hpp>
#include <memory/easy-map.hpp>
#include <panic>
#include <cstring>

namespace OS {

UserCode::UserCode(void * virtCode, size_t codeSize) : physSize(0, 0) {
  alignment = AddressSpace::GetPageAlignment(0);
  physSize.pageSize = AddressSpace::GetPageSize(0);
  physSize.pageCount = codeSize / physSize.pageSize;
  if (codeSize % physSize.pageSize) physSize.pageCount++;
  
  size_t total = physSize.Total();
  physStart = PhysicalAllocator::GetGlobal().Alloc(total, alignment);
  if (!physStart) {
    Panic("UserCode::UserCode() - failed to allocate physical memory");
  }
  
  EasyMap map(physStart, total);
  memcpy((void *)map.GetStart(), virtCode, codeSize);
}

size_t UserCode::GetPageAlignment() {
  return alignment;
}
  
PhysAddr UserCode::GetPhysStart() {
  return physStart;
}

const AddressSpace::Size & UserCode::GetPhysSize() {
  return physSize;
}
  
UserCode::~UserCode() {
  PhysicalAllocator::GetGlobal().Free(physStart);
}

}
