#ifndef __SYSCALL_TIME_HPP__
#define __SYSCALL_TIME_HPP__

#include <arch/general/arg-list.hpp>
#include <syscall/return-value.hpp>

namespace OS {

ReturnValue SyscallGetBootMicroTime();
void SyscallSetTimeout(ArgList & args);
void SyscallSetInfiniteTimeout();
void SyscallClearTimeout(ArgList & args);
void SyscallClearClear();

}

#endif
