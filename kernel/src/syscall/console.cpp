#include <scheduler/general/hold-scope.hpp>
#include <scheduler/general/task.hpp>
#include <scheduler/user/kill-reasons.hpp>
#include <arch/general/user-map.hpp>
#include <arch/general/console.hpp>

namespace OS {

void SyscallPrint(const char * strBuf, uint8_t color, bool bright) {
  HoldScope scope;
  
  if (color > 7) color = 7;
  bright = (bright != false);
  
  Console & console = Console::GetGlobal();
  console.SetColor((Console::Color)color, bright);
  
  UserMap * map = scope.GetTask()->GetUserAddressSpace();
  assert(map != NULL);
  
  while (1) {
    char str[2] = {0, 0};
    map->CopyToKernel((void *)str, (VirtAddr)strBuf, 1);
    if (!str[0]) break;
    console.PrintString(str);
    strBuf++;
  }
}

}
