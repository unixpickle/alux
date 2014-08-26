#include <anarch/api/syscall-args>
#include <anarch/api/syscall-ret>

namespace Alux {

void ExitSyscall(anarch::SyscallArgs & args);
anarch::SyscallRet GetPidSyscall();
anarch::SyscallRet GetUidSyscall();

}
