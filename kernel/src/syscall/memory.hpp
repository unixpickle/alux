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
anarch::SyscallRet FreeSyscall(anarch::SyscallArgs &);

anarch::SyscallRet VMReadSyscall(anarch::SyscallArgs &);
anarch::SyscallRet VMMapSyscall(anarch::SyscallArgs &);
anarch::SyscallRet VMMapAtSyscall(anarch::SyscallArgs &);
anarch::SyscallRet VMUnmapSyscall(anarch::SyscallArgs &);
anarch::SyscallRet VMUnmapAndReserveSyscall(anarch::SyscallArgs &);
anarch::SyscallRet VMReserveSyscall(anarch::SyscallArgs &);
anarch::SyscallRet VMReserveAtSyscall(anarch::SyscallArgs &);
anarch::SyscallRet VMUnreserveSyscall(anarch::SyscallArgs &);
anarch::SyscallRet VMRereserveSyscall(anarch::SyscallArgs &);

}

#endif
