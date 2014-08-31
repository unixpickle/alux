#ifndef __ALUX_SYSCALL_CONSOLE_HPP__
#define __ALUX_SYSCALL_CONSOLE_HPP__

#include <anarch/api/syscall-args>

namespace Alux {

void PrintSyscall(anarch::SyscallArgs & args);
void SetColorSyscall(anarch::SyscallArgs & args);

}

#endif
