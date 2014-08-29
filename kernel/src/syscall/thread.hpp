#ifndef __ALUX_SYSCALL_THREAD_HPP__
#define __ALUX_SYSCALL_THREAD_HPP__

#include <anarch/api/syscall-args>
#include <anarch/api/syscall-ret>

namespace Alux {

void ExitThreadSyscall();
anarch::SyscallRet GetThreadIdSyscall();
anarch::SyscallRet LaunchThreadSyscall(anarch::SyscallArgs &);
void SleepSyscall(anarch::SyscallArgs &);
void SleepInfiniteSyscall();
anarch::SyscallRet WakeupSyscall(anarch::SyscallArgs &);

}

#endif
