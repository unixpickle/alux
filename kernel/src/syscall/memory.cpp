#include <syscall/memory.hpp>
#include <scheduler/general/thread.hpp>
#include <scheduler/general/task.hpp>
#include <scheduler/general/hold-scope.hpp>
#include <scheduler/user/kill-reasons.hpp>
#include <arch/general/hardware-thread.hpp>
#include <arch/general/user-map.hpp>
#include <arch/general/physical-allocator.hpp>
#include <critical>

namespace OS {

ReturnValue SyscallGetPhysicalUsed() {
  HoldScope scope;
  size_t value = PhysicalAllocator::GetGlobal().Used();
  return ReturnValue::NewUInt64((uint64_t)value);
}

ReturnValue SyscallGetPhysicalAvailable() {
  HoldScope scope;
  size_t value = PhysicalAllocator::GetGlobal().Available();
  return ReturnValue::NewUInt64((uint64_t)value);
}

ReturnValue SyscallGetPhysicalTotal() {
  HoldScope scope;
  size_t value = PhysicalAllocator::GetGlobal().Total();
  return ReturnValue::NewUInt64((uint64_t)value);
}

ReturnValue SyscallGetPageSizeCount() {
  AssertCritical();
  int value = AddressSpace::GetPageSizeCount();
  return ReturnValue::NewUInt64((uint64_t)value);
}

ReturnValue SyscallGetPageSize(ArgList & args) {
  AssertCritical();
  int index = args.PopInt();
  if (index < 0 || index >= AddressSpace::GetPageSizeCount()) {
    return ReturnValue::NewUInt64(0);
  }
  size_t value = AddressSpace::GetPageSize((int)index);
  return ReturnValue::NewUInt64((uint64_t)value);
}

ReturnValue SyscallGetPageAlignment(ArgList & args) {
  AssertCritical();
  int index = args.PopInt();
  if (index < 0 || index >= AddressSpace::GetPageSizeCount()) {
    return ReturnValue::NewUInt64(0);
  }
  size_t value = AddressSpace::GetPageAlignment((int)index);
  return ReturnValue::NewUInt64((uint64_t)value);
}

ReturnValue SyscallAllocatePhysical(ArgList & args) {
  HoldScope scope;
  if (scope.GetTask()->GetUID()) {
    scope.Exit(KillReasons::UnauthorizedSyscall);
  }
  
  VirtAddr addrOut = args.PopVirtAddr();
  size_t align = (size_t)args.PopUInt64();
  size_t size = (size_t)args.PopUInt64();
  
  PhysAddr result = PhysicalAllocator::GetGlobal().Alloc(align, size, NULL);
  if (!result) return ReturnValue::NewBool(false);
  
  UserMap * map = scope.GetTask()->GetUserAddressSpace();
  assert(map != NULL);
  
  // note that this might not actually return if the task gave us a pointer
  // it doesn't own, in which case physical memory will be leaked
  map->CopyFromKernel(addrOut, (void *)&result, sizeof(result));
  
  return ReturnValue::NewBool(false);
}

void SyscallFreePhysical(ArgList & args) {
  HoldScope scope;
  if (scope.GetTask()->GetUID()) {
    scope.Exit(KillReasons::UnauthorizedSyscall);
  }
  
  PhysAddr addr = args.PopPhysAddr();
  PhysicalAllocator::GetGlobal().Free(addr);
}

ReturnValue SyscallMapPhysical(ArgList & args) {
  HoldScope scope;
  if (scope.GetTask()->GetUID()) {
    scope.Exit(KillReasons::UnauthorizedSyscall);
  }
  
  size_t pageSize = (size_t)args.PopUInt64();
  size_t pageCount = (size_t)args.PopUInt64();
  uint32_t flags = args.PopUInt32();
  PhysAddr physAddr = args.PopPhysAddr();
  VirtAddr addrOut = args.PopVirtAddr();
  
  UserMap * map = scope.GetTask()->GetUserAddressSpace();
  assert(map != NULL);
  
  bool exec = (flags & 1) != 0;
  bool writable = (flags & 2) != 0;
  
  AddressSpace::MapInfo info(pageSize, pageCount, physAddr, exec, writable);
  VirtAddr result;
  if (!map->Map(info, result)) return ReturnValue::NewBool(false);
  
  // we need to cast the result to a pointer before copying it (for future
  // platforms) -- TODO: make everything a VirtAddr all over the place
  map->CopyFromKernel(addrOut, (void *)&result, sizeof(result));
  return ReturnValue::NewBool(true);
}

void SyscallUnmapPhysical(ArgList & args) {
  HoldScope scope;
  if (scope.GetTask()->GetUID()) {
    scope.Exit(KillReasons::UnauthorizedSyscall);
  }
  
  size_t pageSize = (size_t)args.PopUInt64();
  size_t pageCount = (size_t)args.PopUInt64();
  VirtAddr address = args.PopVirtAddr();
  
  UserMap * map = scope.GetTask()->GetUserAddressSpace();
  assert(map != NULL);
  
  map->Unmap(address, AddressSpace::Size(pageSize, pageCount));
}

}
