#include "memory.hpp"
#include "../tasks/hold-scope.hpp"
#include <anarch/api/user-map>
#include <anarch/api/domain>

namespace Alux {

anarch::SyscallRet CountPageSizesSyscall() {
  return anarch::SyscallRet::Integer(anarch::UserMap::GetPageSizeCount());
}

anarch::SyscallRet GetPageSizeSyscall(anarch::SyscallArgs & args) {
  int idx = args.PopInt();
  if (idx < 0 || idx >= anarch::UserMap::GetPageSizeCount()) {
    return anarch::SyscallRet::Integer(0);
  }
  return anarch::SyscallRet::VirtSize(anarch::UserMap::GetPageSize(idx));
}

anarch::SyscallRet GetPageAlignSyscall(anarch::SyscallArgs & args) {
  int idx = args.PopInt();
  if (idx < 0 || idx >= anarch::UserMap::GetPageSizeCount()) {
    return anarch::SyscallRet::Integer(0);
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
    // they're not root
    scope.ExitTask(Task::KillReasonPermissions);
  }
  
  PhysSize size = args.PopPhysSize();
  PhysSize align = args.PopPhysSize();
  
  PhysAddr result;
  if (!anarch::Domain::GetCurrent().AllocPhys(result, size, align)) {
    return anarch::SyscallRet::Phys(0);
  }
  assert(result + 1 != 0);
  
  // add one to the result so that they know it wasn't a failure
  return anarch::SyscallRet::Phys(result + 1);
}

void FreeSyscall(anarch::SyscallArgs & args) {
  HoldScope scope;
  if (scope.GetTask().GetUserIdentifier() != 0) {
    // they're not root
    scope.ExitTask(Task::KillReasonPermissions);
  }

  PhysAddr addr = args.PopPhysAddr();
  anarch::Domain::GetCurrent().FreePhys(addr);
}

}
