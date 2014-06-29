#ifndef __SYSCALL_SCHEDULER_HPP__
#define __SYSCALL_SCHEDULER_HPP__

namespace OS {

uint64_t SyscallLaunchThread(void * address, void * argument);
void SyscallFork(void * address, void * argument);
void SyscallExit(bool wasError);
void SyscallThreadExit();
uint64_t SyscallGetPID();
uint64_t SyscallGetThreadID();
uint64_t SyscallGetTaskCount();
uint64_t SyscallGetThreadCount();

}

#endif
