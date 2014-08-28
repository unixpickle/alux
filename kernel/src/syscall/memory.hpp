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

anarch::SyscallRet VMReadSyscall(anarch::SyscallArgs &);
anarch::SyscallRet VMMapSyscall(anarch::SyscallArgs &);
void VMMapAtSyscall(anarch::SyscallArgs &);
void VMUnmapSyscall(anarch::SyscallArgs &);
void VMUnmapAndReserveSyscall(anarch::SyscallArgs &);
anarch::SyscallRet VMReserveSyscall(anarch::SyscallArgs &);
void VMReserveAtSyscall(anarch::SyscallArgs &);
void VMUnreserveSyscall(anarch::SyscallArgs &);
void VMRereserveSyscall(anarch::SyscallArgs &);

}

#endif
