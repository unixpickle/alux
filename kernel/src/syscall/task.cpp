#include "task.hpp"
#include "../tasks/hold-scope.hpp"

namespace Alux {

void ExitSyscall(anarch::SyscallArgs & args) {
  bool aborted = args.PopBool();
  HoldScope scope;
  scope.ExitTask(aborted ? 1 : 0);
}

}
