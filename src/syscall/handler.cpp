#include <syscall/handler.hpp>
#include <syscall/console.hpp>
#include <syscall/scheduler.hpp>
#include <syscall/time.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <scheduler/general/hold-scope.hpp>
#include <scheduler/general/task.hpp>
#include <scheduler/user/kill-reasons.hpp>
#include <arch/general/hardware-thread.hpp>
#include <iostream>
#include <critical>

namespace OS {

uint64_t SyscallHandler(uint16_t callNumber,
                        void * arg1,
                        void * arg2,
                        void * arg3,
                        uint64_t arg4,
                        uint64_t arg5) {
  AssertCritical();
  
  // just for now, until I need this argument for something
  (void)arg3;
  
  switch (callNumber) {
    case 0:
      SyscallPrint((const char *)arg1, (uint8_t)arg4, (bool)arg5);
      break;
    case 1:
      return SyscallLaunchThread(arg1, arg2);
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
    case 5:
      return SyscallGetPID();
      break;
    case 6:
      return SyscallGetThreadID();
      break;
    case 7:
      return SyscallGetBootMicroTime();
      break;
    case 8:
      SyscallSetTimeout(arg4);
      break;
    case 9:
      SyscallSetInfiniteTimeout();
      break;
    case 10:
      SyscallClearTimeout(arg4);
      break;
    case 11:
      SyscallClearClear();
      break;
    case 12:
      return SyscallGetTaskCount();
      break;
    case 13:
      return SyscallGetThreadCount();
      break;
    default:
      Task::Exit(KillReasons::InvalidSyscall);
      break;
  }
  return 0;
}

}
