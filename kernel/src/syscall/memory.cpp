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

uint64_t SyscallGetPhysicalUsed() {
  HoldScope scope;
  return (uint64_t)PhysicalAllocator::GetGlobal().Used();
}

uint64_t SyscallGetPhysicalAvailable() {
  HoldScope scope;
  return (uint64_t)PhysicalAllocator::GetGlobal().Available();
}

uint64_t SyscallGetPhysicalTotal() {
  HoldScope scope;
  return (uint64_t)PhysicalAllocator::GetGlobal().Total();
}

uint64_t SyscallGetPageSizeCount() {
  AssertCritical();
  return (uint64_t)AddressSpace::GetPageSizeCount();
}

uint64_t SyscallGetPageSize(uint64_t _index) {
  AssertCritical();
  int index = (int)_index;
  if (index < 0 || index >= AddressSpace::GetPageSizeCount()) {
    return 0;
  }
  return (uint64_t)AddressSpace::GetPageSize((int)index);
}

uint64_t SyscallGetPageAlignment(uint64_t _index) {
  AssertCritical();
  int index = (int)_index;
  if (index < 0 || index >= AddressSpace::GetPageSizeCount()) {
    return 0;
  }
  return (uint64_t)AddressSpace::GetPageAlignment((int)index);
}

bool SyscallAllocatePhysical(PhysAddr * addrOut, uint64_t size,
                                 uint64_t align) {
  HoldScope scope;
  if (scope.GetTask()->GetUID()) {
    scope.Exit(KillReasons::UnauthorizedSyscall);
  }
  
  PhysAddr result = PhysicalAllocator::GetGlobal().Alloc(align, size, NULL);
  if (!result) return false;
  
  UserMap * map = scope.GetTask()->GetUserAddressSpace();
  assert(map != NULL);
  
  // note that this might not actually return if the task gave us a pointer
  // it doesn't own, in which case physical memory will be leaked
  map->CopyFromKernel((VirtAddr)addrOut, (void *)&result, sizeof(result));
  
  return true;
}

void SyscallFreePhysical(PhysAddr * addrIn) {
  HoldScope scope;
  if (scope.GetTask()->GetUID()) {
    scope.Exit(KillReasons::UnauthorizedSyscall);
  }
  
  UserMap * map = scope.GetTask()->GetUserAddressSpace();
  assert(map != NULL);
  
  PhysAddr addr;
  map->CopyToKernel((void *)&addr, (PhysAddr)addrIn, sizeof(addr));
  
  PhysicalAllocator::GetGlobal().Free(addr);
}

bool SyscallMapPhysical(uint64_t pageSize, uint64_t pageCount,
                        uint64_t * flagsPtr, PhysAddr * addrIn,
                        void ** addressOut) {
  HoldScope scope;
  if (scope.GetTask()->GetUID()) {
    scope.Exit(KillReasons::UnauthorizedSyscall);
  }
  
  UserMap * map = scope.GetTask()->GetUserAddressSpace();
  assert(map != NULL);
  
  PhysAddr physAddr;
  map->CopyToKernel((void *)&physAddr, (VirtAddr)addrIn, sizeof(physAddr));
  
  uint64_t flags;
  map->CopyToKernel((void *)&flags, (VirtAddr)flagsPtr, sizeof(flags));
  
  bool exec = (flags & 1) != 0;
  bool writable = (flags & 2) != 0;
  
  AddressSpace::MapInfo info(pageSize, pageCount, physAddr, exec, writable);
  VirtAddr result;
  if (!map->Map(info, result)) return false;
  
  // we need to cast the result to a pointer before copying it (for future
  // platforms) -- TODO: make everything a VirtAddr all over the place
  void * resultPtr = (void *)result;
  map->CopyFromKernel((VirtAddr)addressOut, (void *)&resultPtr,
                      sizeof(result));
  return true;
}

void SyscallUnmapPhysical(uint64_t pageSize, uint64_t pageCount,
                          void * address) {
  HoldScope scope;
  if (scope.GetTask()->GetUID()) {
    scope.Exit(KillReasons::UnauthorizedSyscall);
  }
  
  UserMap * map = scope.GetTask()->GetUserAddressSpace();
  assert(map != NULL);
  
  map->Unmap((VirtAddr)address, AddressSpace::Size(pageSize, pageCount));
}

}
