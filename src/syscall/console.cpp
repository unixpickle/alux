#include <scheduler/general/hold-scope.hpp>
#include <scheduler/general/task.hpp>
#include <arch/general/user-map.hpp>
#include <iostream>
#include <critical>

#include <panic> // TODO: delete this

namespace OS {

void SyscallPrint(const char * strBuf) {
  SetCritical(false);
  HoldScope scope;
  
  UserMap * map = scope.GetTask()->GetUserAddressSpace();
  assert(map != NULL);
  while (1) {
    if (!map->OwnsRange((VirtAddr)strBuf, 1)) {
      // TODO: kill the task here
      Panic("printed from invalid memory");
    }
    if (!*strBuf) break;
    char str[2] = {*strBuf, 0};
    cout << str;
    strBuf++;
  }
  
  SetCritical(true);
}

}
