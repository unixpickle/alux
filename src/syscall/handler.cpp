#include <syscall/handler.hpp>
#include <syscall/console.hpp>
#include <syscall/scheduler.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <scheduler/general/hold-scope.hpp>
#include <scheduler/user/kill-reasons.hpp>
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
  (void)arg3;
  (void)arg5;
  
  switch (callNumber) {
  case 0:
    SyscallPrint((const char *)arg1);
    break;
  case 1:
    SyscallLaunchThread(arg1, arg2);
    break;
  case 2:
    SyscallFork(arg1, arg2);
    break;
  case 3:
    SyscallExit(arg4 != 0);
    break;
  case 4:
    SyscallThreadExit();
    break;
  default:
    Scheduler::GetGlobal().ExitTask(KillReasons::InvalidSyscall);
    break;
  }
}

}
