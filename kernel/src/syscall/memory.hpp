#ifndef __ALUX_SYSCALL_MEMORY_HPP__
#define __ALUX_SYSCALL_MEMORY_HPP__

#include <anarch/types>
#include <anarch/api/syscall-ret>
#include <anarch/api/syscall-args>

namespace Alux {

// general UserMap info
anarch::SyscallRet CountPageSizesSyscall();
anarch::SyscallRet GetPageSizeSyscall(anarch::SyscallArgs &);
anarch::SyscallRet GetPageAlignSyscall(anarch::SyscallArgs &);
anarch::SyscallRet GetVMCapabilitiesSyscall();

// physical memory management
anarch::SyscallRet AllocateSyscall(anarch::SyscallArgs &);
void FreeSyscall(anarch::SyscallArgs &);

}

#endif
