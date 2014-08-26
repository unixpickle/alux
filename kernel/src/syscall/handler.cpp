#include "handler.hpp"
#include "console.hpp"
#include "memory.hpp"
#include "task.hpp"
#include <anarch/stream>
#include <anarch/critical>

namespace Alux {

anarch::SyscallRet SyscallHandler(uint16_t number,
                                  anarch::SyscallArgs & args) {
  AssertCritical();
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
    default:
      anarch::cerr << "unknown SyscallHandler(" << number << ", ...)"
        << anarch::endl;
      break;
  }
  return anarch::SyscallRet();
}

}