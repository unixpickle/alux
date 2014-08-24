#include "console.hpp"
#include "../tasks/hold-scope.hpp"
#include "../tasks/user-task.hpp"
#include <anarch/api/console>

namespace Alux {

void PrintSyscall(anarch::SyscallArgs & args) {
  HoldScope scope;
  assert(scope.GetTask().IsUserTask());
  
  VirtAddr addr = args.PopVirtAddr();
  UserTask & task = static_cast<UserTask &>(scope.GetTask());
  
  while (1) {
    char buffer[2] = {0, 0};
    task.GetMemoryMap().CopyToKernel((void *)buffer, addr++, 1);
    if (!buffer[0]) break;
    anarch::Console::GetGlobal().PrintString(buffer);
  }
}

}