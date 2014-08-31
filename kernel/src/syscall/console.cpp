#include "console.hpp"
#include "../tasks/hold-scope.hpp"
#include "../tasks/user-task.hpp"
#include <anarch/api/console>

namespace Alux {

void PrintSyscall(anarch::SyscallArgs & args) {
  HoldScope scope;
  
  VirtAddr addr = args.PopVirtAddr();
  UserTask & task = scope.GetUserTask();
  
  while (1) {
    char buffer[2] = {0, 0};
    task.GetMemoryMap().CopyToKernel((void *)buffer, addr++, 1);
    if (!buffer[0]) break;
    anarch::Console::GetGlobal().PrintString(buffer);
  }
}

void SetColorSyscall(anarch::SyscallArgs & args) {
  int color = args.PopInt() & 7;
  bool bright = args.PopBool();
  
  anarch::Console::Color c = (anarch::Console::Color)color;
  anarch::Console::GetGlobal().SetColor(c, bright);
}

}