#ifndef __ALUX_SYSCALL_TASK_HPP__
#define __ALUX_SYSCALL_TASK_HPP__

#include <anarch/api/syscall-args>
#include <anarch/api/syscall-ret>

namespace Alux {

void ExitSyscall(anarch::SyscallArgs & args);
anarch::SyscallRet GetPidSyscall();
anarch::SyscallRet GetUidSyscall();

}

#endif
