#include <syscall/handler.hpp>
#include <syscall/console.hpp>
#include <syscall/scheduler.hpp>
#include <syscall/time.hpp>
#include <syscall/memory.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <scheduler/general/hold-scope.hpp>
#include <scheduler/general/task.hpp>
#include <scheduler/user/kill-reasons.hpp>
#include <arch/general/hardware-thread.hpp>
#include <critical>

namespace OS {

ReturnValue SyscallHandler(uint16_t callNumber, ArgList & args) {
  AssertCritical();
  
  switch (callNumber) {
    case 0:
      SyscallPrint(args);
      break;
    case 1:
      return SyscallLaunchThread(args);
      break;
    case 2:
      SyscallFork(args);
      break;
    case 3:
      SyscallExit(args);
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
      SyscallSetTimeout(args);
      break;
    case 9:
      SyscallSetInfiniteTimeout();
      break;
    case 10:
      SyscallClearTimeout(args);
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
    case 14:
      return SyscallGetPhysicalUsed();
      break;
    case 15:
      return SyscallGetPhysicalAvailable();
      break;
    case 16:
      return SyscallGetPhysicalTotal();
      break;
    case 17:
      return SyscallGetPageSizeCount();
      break;
    case 18:
      return SyscallGetPageSize(args);
      break;
    case 19:
      return SyscallGetPageAlignment(args);
      break;
    case 20:
      return SyscallAllocatePhysical(args);
      break;
    case 21:
      SyscallFreePhysical(args);
      break;
    case 22:
      return SyscallMapPhysical(args);
      break;
    case 23:
      SyscallUnmapPhysical(args);
      break;
    default:
      Task::Exit(KillReasons::InvalidSyscall);
      break;
  }
  return ReturnValue::NewVoid();
}

}
