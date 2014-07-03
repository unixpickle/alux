#ifndef __SYSCALL_SCHEDULER_HPP__
#define __SYSCALL_SCHEDULER_HPP__

#include <arch/general/arg-list.hpp>
#include <syscall/return-value.hpp>

namespace OS {

ReturnValue SyscallLaunchThread(ArgList & args);
void SyscallFork(ArgList & list);
void SyscallExit(ArgList & list);
void SyscallThreadExit();
ReturnValue SyscallGetPID();
ReturnValue SyscallGetThreadID();
ReturnValue SyscallGetTaskCount();
ReturnValue SyscallGetThreadCount();

}

#endif
