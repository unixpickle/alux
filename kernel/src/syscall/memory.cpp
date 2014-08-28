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

/*anarch::MemoryMap::Attributes DecodeAttributes(int arg) {
  anarch::MemoryMap::Attributes result;
  result.executable = ((arg & 1) != 0);
  result.writable = ((arg & 2) != 0);
  result.cachable = ((arg & 4) != 0);
  return result;
}*/

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
  VirtAddr physPtr = args.PopVirtAddr();
  VirtAddr sizePtr = args.PopVirtAddr();
  VirtAddr virt = args.PopVirtAddr();
  
  PhysAddr resPhys;
  anarch::MemoryMap::Attributes resAttributes;
  size_t resVirt;
  if (!map.Read(&resPhys, &resAttributes, &resVirt, virt)) {
    return anarch::SyscallRet::Error(SyscallErrorNoMapping);
  }
  
  map.CopyFromKernel(physPtr, &resPhys, sizeof(resPhys));
  map.CopyFromKernel(sizePtr, &resVirt, sizeof(resVirt));  
  return anarch::SyscallRet::Integer(EncodeAttributes(resAttributes));
}

anarch::SyscallRet VMMapSyscall(anarch::SyscallArgs &) {
  return anarch::SyscallRet::Boolean(false);
}

void VMMapAtSyscall(anarch::SyscallArgs &) {
  
}

void VMUnmapSyscall(anarch::SyscallArgs &) {
  
}

void VMUnmapAndReserveSyscall(anarch::SyscallArgs &) {
  
}

anarch::SyscallRet VMReserveSyscall(anarch::SyscallArgs &) {
  return anarch::SyscallRet::Boolean(false);
}

void VMReserveAtSyscall(anarch::SyscallArgs &) {
  
}

void VMUnreserveSyscall(anarch::SyscallArgs &) {
  
}

void VMRereserveSyscall(anarch::SyscallArgs &) {
  
}

}
