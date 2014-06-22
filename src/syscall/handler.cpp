#include <syscall/handler.hpp>
#include <syscall/console.hpp>
#include <scheduler/general/hold-scope.hpp>
#include <arch/general/hardware-thread.hpp>
#include <iostream>
#include <critical>

namespace OS {

void SyscallHandler(uint16_t callNumber,
                    void * arg1,
                    void * arg2,
                    void * arg3,
                    uint64_t arg4,
                    uint64_t arg5) {
  AssertCritical();
  
  // just for now, until all of these arguments are passed to things
  (void)arg2;
  (void)arg3;
  (void)arg4;
  (void)arg5;
  
  switch (callNumber) {
  case 0:
    SyscallPrint((const char *)arg1);
    break;
  }
}

}
