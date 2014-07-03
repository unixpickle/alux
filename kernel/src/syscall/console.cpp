#include <syscall/console.hpp>
#include <scheduler/general/hold-scope.hpp>
#include <scheduler/general/task.hpp>
#include <scheduler/user/kill-reasons.hpp>
#include <arch/general/user-map.hpp>
#include <arch/general/console.hpp>

namespace OS {

void SyscallPrint(ArgList & list) {
  HoldScope scope;
  
  VirtAddr strBuf = list.PopVirtAddr();
  uint8_t color = (uint8_t)list.PopUInt64();
  bool bright = list.PopBool() != false;
  
  if (color > 7) color = 7;
  
  Console & console = Console::GetGlobal();
  console.SetColor((Console::Color)color, bright);
  
  UserMap * map = scope.GetTask()->GetUserAddressSpace();
  assert(map != NULL);
  
  while (1) {
    char str[2] = {0, 0};
    map->CopyToKernel((void *)str, strBuf, 1);
    if (!str[0]) break;
    console.PrintString(str);
    strBuf++;
  }
}

}
