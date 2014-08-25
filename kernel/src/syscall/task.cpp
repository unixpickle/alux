#include "task.hpp"
#include "../tasks/hold-scope.hpp"
#include <anarch/critical>

namespace Alux {

void ExitSyscall(anarch::SyscallArgs & args) {
  bool aborted = args.PopBool();
  HoldScope scope;
  scope.ExitTask(aborted ? 1 : 0);
}

anarch::SyscallRet GetPidSyscall() {
  AssertCritical();
  Task & t = Thread::GetCurrent()->GetTask();
  return anarch::SyscallRet::Integer32((uint32_t)t.GetIdentifier());
}

}
