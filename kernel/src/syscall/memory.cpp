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

uint64_t SyscallAllocatePhysical(PhysAddr * addrOut, uint64_t size,
                                 uint64_t align) {
  HoldScope scope;
  if (scope.GetTask()->GetUID()) {
    scope.Exit(KillReasons::UnauthorizedSyscall);
  }
  
  PhysAddr result = PhysicalAllocator::GetGlobal().Alloc(align, size, NULL);
  if (!result) return 0;
  
  UserMap * map = scope.GetTask()->GetUserAddressSpace();
  assert(map != NULL);
  
  // note that this might not actually return if the task triggers a page
  // fault, in which case the physical memory will be leaked
  bool res = map->CopyFromKernel((VirtAddr)addrOut, (void *)&result,
                                 sizeof(result));
  if (!res) {
    // why bother freeing memory here, we might as well just cry :'(
    scope.Exit(KillReasons::UnownedMemory);
  }
  return 1;
}

void SyscallMapPhysical(uint64_t pageSize, uint64_t pageCount,
                        uint64_t * flagsPtr, PhysAddr * addrIn,
                        void ** addressOut) {
  HoldScope scope;
  if (scope.GetTask()->GetUID()) {
    scope.Exit(KillReasons::UnauthorizedSyscall);
  }
  
  UserMap * map = scope.GetTask()->GetUserAddressSpace();
  assert(map != NULL);
  
  PhysAddr physAddr;
  bool res = map->CopyToKernel((void *)&physAddr, (VirtAddr)addrIn,
                               sizeof(physAddr));
  if (!res) {
    scope.Exit(KillReasons::UnownedMemory);
  }
  
  uint64_t flags;
  res = map->CopyToKernel((void *)&flags, (VirtAddr)flagsPtr, sizeof(flags));
  if (!res) {
    scope.Exit(KillReasons::UnownedMemory);
  }
  
  bool exec = (flags & 1) != 0;
  bool writable = (flags & 2) != 0;
  AddressSpace::MapInfo info(pageSize, pageCount, physAddr, exec, writable);
  void * result = (void *)map->Map(info);
  res = map->CopyFromKernel((VirtAddr)addressOut, (void *)&result,
                            sizeof(result));
  if (!res) {
    scope.Exit(KillReasons::UnownedMemory);
  }
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
