#include <scheduler/general/hold-scope.hpp>
#include <scheduler/general/task.hpp>
#include <scheduler/user/kill-reasons.hpp>
#include <arch/general/user-map.hpp>
#include <iostream>
#include <critical>

namespace OS {

void SyscallPrint(const char * strBuf) {
  HoldScope scope;
  
  UserMap * map = scope.GetTask()->GetUserAddressSpace();
  assert(map != NULL);
  while (1) {
    char str[2] = {0, 0};
    if (!map->CopyToKernel((void *)str, (VirtAddr)strBuf, 1)) {
      scope.Exit(KillReasons::UnownedMemory);
    }
    if (!str[0]) break;
    cout << str;
    strBuf++;
  }
}

}
