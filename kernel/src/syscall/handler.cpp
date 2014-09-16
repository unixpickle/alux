#include "handler.hpp"
#include "console.hpp"
#include "memory.hpp"
#include "thread.hpp"
#include "time.hpp"
#include "task.hpp"
#include "port.hpp"
#include <anarch/stream>
#include <anarch/critical>

namespace Alux {

anarch::SyscallRet SyscallHandler(uint16_t number,
                                  anarch::SyscallArgs & args) {
  AssertCritical();
  // You might be like "holy crap this is terrible". I'd respond to you as
  // follows: out of all of the syscall mechanisms I've seen, this is the most
  // clear, efficient, and unified way to do it.
  switch (number) {
    case 0:
      PrintSyscall(args);
      break;
    case 1:
      ExitSyscall(args);
      break;
    case 2:
      return GetPidSyscall();
    case 3:
      return GetUidSyscall();
    case 4:
      return CountPageSizesSyscall();
    case 5:
      return GetPageSizeSyscall(args);
    case 6:
      return GetPageAlignSyscall(args);
    case 7:
      return GetVMCapabilitiesSyscall();
    case 8:
      return AllocateSyscall(args);
    case 9:
      FreeSyscall(args);
      break;
    case 10:
      return GetNanoTimeSyscall();
    case 11:
      return VMReadSyscall(args);
    case 12:
      return VMMapSyscall(args);
    case 13:
      return VMMapAtSyscall(args);
    case 14:
      return VMUnmapSyscall(args);
    case 15:
      return VMUnmapAndReserveSyscall(args);
    case 16:
      return VMReserveSyscall(args);
    case 17:
      return VMReserveAtSyscall(args);
    case 18:
      return VMUnreserveSyscall(args);
    case 19:
      return VMRereserveSyscall(args);
    case 20:
      ExitThreadSyscall();
      break;
    case 21:
      return GetThreadIdSyscall();
    case 22:
      return LaunchThreadSyscall(args);
    case 23:
      SleepSyscall(args);
      break;
    case 24:
      SleepInfiniteSyscall();
      break;
    case 25:
      return WakeupSyscall(args);
    case 26:
      SetColorSyscall(args);
      break;
    case 27:
      return CreatePortSyscall();
    case 28:
      return DestroyPortSyscall(args);
    default:
      anarch::cerr << "unknown SyscallHandler(" << number << ", ...)"
        << anarch::endl;
      break;
  }
  return anarch::SyscallRet::Empty();
}

}