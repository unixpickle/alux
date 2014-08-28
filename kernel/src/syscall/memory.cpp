#include "memory.hpp"
#include "errors.hpp"
#include "../tasks/hold-scope.hpp"
#include <anarch/api/user-map>
#include <anarch/api/domain>

namespace Alux {

namespace {

int EncodeAttributes(anarch::MemoryMap::Attributes & attrs) {
  int res = 0;
  if (attrs.executable) res |= 1;
  if (attrs.writable) res |= 2;
  if (attrs.cachable) res |= 4;
  return res;
}

anarch::MemoryMap::Attributes DecodeAttributes(int arg) {
  anarch::MemoryMap::Attributes result;
  result.executable = ((arg & 1) != 0);
  result.writable = ((arg & 2) != 0);
  result.cachable = ((arg & 4) != 0);
  return result;
}

}

anarch::SyscallRet CountPageSizesSyscall() {
  return anarch::SyscallRet::Integer(anarch::UserMap::GetPageSizeCount());
}

anarch::SyscallRet GetPageSizeSyscall(anarch::SyscallArgs & args) {
  int idx = args.PopInt();
  if (idx < 0 || idx >= anarch::UserMap::GetPageSizeCount()) {
    return anarch::SyscallRet::Error(SyscallErrorIndex);
  }
  return anarch::SyscallRet::VirtSize(anarch::UserMap::GetPageSize(idx));
}

anarch::SyscallRet GetPageAlignSyscall(anarch::SyscallArgs & args) {
  int idx = args.PopInt();
  if (idx < 0 || idx >= anarch::UserMap::GetPageSizeCount()) {
    return anarch::SyscallRet::Error(SyscallErrorIndex);
  }
  return anarch::SyscallRet::VirtSize(anarch::UserMap::GetPageSizeAlign(idx));
}

anarch::SyscallRet GetVMCapabilitiesSyscall() {
  anarch::UserMap::Capabilities caps = anarch::UserMap::GetCapabilities();
  int result = 0;
  if (caps.placementReserve) result |= 1;
  if (caps.placementMap) result |= 2;
  if (caps.executableFlag) result |= 4;
  if (caps.writableFlag) result |= 8;
  if (caps.cachableFlag) result |= 0x10;
  return anarch::SyscallRet::Integer(result);
}

anarch::SyscallRet AllocateSyscall(anarch::SyscallArgs & args) {
  HoldScope scope;
  if (scope.GetTask().GetUserIdentifier() != 0) {
    return anarch::SyscallRet::Error(SyscallErrorPermissions);
  }
  
  PhysSize size = args.PopPhysSize();
  PhysSize align = args.PopPhysSize();
  
  PhysAddr result;
  if (!anarch::Domain::GetCurrent().AllocPhys(result, size, align)) {
    return anarch::SyscallRet::Error(SyscallErrorNoMemory);
  }
  return anarch::SyscallRet::Phys(result);
}

void FreeSyscall(anarch::SyscallArgs & args) {
  HoldScope scope;
  if (scope.GetTask().GetUserIdentifier() != 0) {
    scope.ExitTask(Task::KillReasonPermissions);
  }

  PhysAddr addr = args.PopPhysAddr();
  anarch::Domain::GetCurrent().FreePhys(addr);
}

anarch::SyscallRet VMReadSyscall(anarch::SyscallArgs & args) {
  HoldScope scope;
  if (scope.GetTask().GetUserIdentifier() != 0) {
    return anarch::SyscallRet::Error(SyscallErrorPermissions);
  }
  
  anarch::UserMap & map = scope.GetUserTask().GetMemoryMap();
  VirtAddr output1 = args.PopVirtAddr();
  VirtAddr output2 = args.PopVirtAddr();
  VirtAddr address = args.PopVirtAddr();
  
  PhysAddr result1;
  anarch::MemoryMap::Attributes attributes;
  size_t result2;
  
  if (!map.Read(&result1, &attributes, &result2, address)) {
    return anarch::SyscallRet::Error(SyscallErrorNoMapping);
  }
  
  map.CopyFromKernel(output1, &result1, sizeof(result1));
  map.CopyFromKernel(output2, &result2, sizeof(result2));
  return anarch::SyscallRet::Integer(EncodeAttributes(attributes));
}

anarch::SyscallRet VMMapSyscall(anarch::SyscallArgs & args) {
  HoldScope scope;
  if (scope.GetTask().GetUserIdentifier() != 0) {
    return anarch::SyscallRet::Error(SyscallErrorPermissions);
  }
  
  anarch::UserMap & map = scope.GetUserTask().GetMemoryMap();
  PhysAddr phys = args.PopPhysAddr();
  size_t pageSize = args.PopVirtSize();
  size_t pageCount = args.PopVirtSize();
  int encodedAttributes = args.PopInt();
  
  anarch::MemoryMap::Attributes attrs = DecodeAttributes(encodedAttributes);
  anarch::MemoryMap::Size size(pageSize, pageCount);
  
  VirtAddr result;
  if (!map.Map(result, phys, size, attrs)) {
    return anarch::SyscallRet::Error(SyscallErrorNoVMSpace);
  }
  return anarch::SyscallRet::Virt(result);
}

void VMMapAtSyscall(anarch::SyscallArgs & args) {
  HoldScope scope;
  if (scope.GetTask().GetUserIdentifier() != 0) {
    scope.ExitTask(Task::KillReasonPermissions);
  }
  
  anarch::UserMap & map = scope.GetUserTask().GetMemoryMap();
  VirtAddr dest = args.PopVirtAddr();
  PhysAddr phys = args.PopPhysAddr();
  size_t pageSize = args.PopVirtSize();
  size_t pageCount = args.PopVirtSize();
  int encodedAttributes = args.PopInt();
  
  anarch::MemoryMap::Attributes attrs = DecodeAttributes(encodedAttributes);
  anarch::MemoryMap::Size size(pageSize, pageCount);
  
  map.MapAt(dest, phys, size, attrs);
}

void VMUnmapSyscall(anarch::SyscallArgs & args) {
  HoldScope scope;
  if (scope.GetTask().GetUserIdentifier() != 0) {
    scope.ExitTask(Task::KillReasonPermissions);
  }
  
  anarch::UserMap & map = scope.GetUserTask().GetMemoryMap();
  VirtAddr addr = args.PopVirtAddr();
  size_t pageSize = args.PopVirtSize();
  size_t pageCount = args.PopVirtSize();
  map.Unmap(addr, anarch::MemoryMap::Size(pageSize, pageCount));
}

void VMUnmapAndReserveSyscall(anarch::SyscallArgs & args) {
  HoldScope scope;
  if (scope.GetTask().GetUserIdentifier() != 0) {
    scope.ExitTask(Task::KillReasonPermissions);
  }
  
  anarch::UserMap & map = scope.GetUserTask().GetMemoryMap();
  VirtAddr addr = args.PopVirtAddr();
  size_t pageSize = args.PopVirtSize();
  size_t pageCount = args.PopVirtSize();
  map.UnmapAndReserve(addr, anarch::MemoryMap::Size(pageSize, pageCount));
}

anarch::SyscallRet VMReserveSyscall(anarch::SyscallArgs & args) {
  HoldScope scope;
  if (scope.GetTask().GetUserIdentifier() != 0) {
    return anarch::SyscallRet::Error(SyscallErrorPermissions);
  }
  
  anarch::UserMap & map = scope.GetUserTask().GetMemoryMap();
  size_t pageSize = args.PopVirtSize();
  size_t pageCount = args.PopVirtSize();
  
  VirtAddr result;
  if (!map.Reserve(result, anarch::MemoryMap::Size(pageSize, pageCount))) {
    return anarch::SyscallRet::Error(SyscallErrorNoVMSpace);
  }
  return anarch::SyscallRet::Virt(result);
}

void VMReserveAtSyscall(anarch::SyscallArgs & args) {
  HoldScope scope;
  if (scope.GetTask().GetUserIdentifier() != 0) {
    scope.ExitTask(Task::KillReasonPermissions);
  }
  
  anarch::UserMap & map = scope.GetUserTask().GetMemoryMap();
  VirtAddr dest = args.PopVirtAddr();
  size_t pageSize = args.PopVirtSize();
  size_t pageCount = args.PopVirtSize();
  
  map.ReserveAt(dest, anarch::MemoryMap::Size(pageSize, pageCount));
}

void VMUnreserveSyscall(anarch::SyscallArgs & args) {
  HoldScope scope;
  if (scope.GetTask().GetUserIdentifier() != 0) {
    scope.ExitTask(Task::KillReasonPermissions);
  }
  
  anarch::UserMap & map = scope.GetUserTask().GetMemoryMap();
  VirtAddr dest = args.PopVirtAddr();
  size_t pageSize = args.PopVirtSize();
  size_t pageCount = args.PopVirtSize();
  
  map.Unreserve(dest, anarch::MemoryMap::Size(pageSize, pageCount));
}

void VMRereserveSyscall(anarch::SyscallArgs & args) {
  HoldScope scope;
  if (scope.GetTask().GetUserIdentifier() != 0) {
    scope.ExitTask(Task::KillReasonPermissions);
  }
  
  anarch::UserMap & map = scope.GetUserTask().GetMemoryMap();
  VirtAddr dest = args.PopVirtAddr();
  size_t pageSize = args.PopVirtSize();
  size_t pageCount = args.PopVirtSize();
  size_t newPageSize = args.PopVirtSize();
  
  map.Rereserve(dest, anarch::MemoryMap::Size(pageSize, pageCount),
                newPageSize);
}

}
