#include "handler.hpp"
#include "console.hpp"
#include "memory.hpp"
#include "time.hpp"
#include "task.hpp"
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
    default:
      anarch::cerr << "unknown SyscallHandler(" << number << ", ...)"
        << anarch::endl;
      break;
  }
  return anarch::SyscallRet();
}

}