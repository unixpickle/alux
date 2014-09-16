#ifndef __ALUX_SYSCALL_PORT_HPP__
#define __ALUX_SYSCALL_PORT_HPP__

#include <anarch/api/syscall-args>
#include <anarch/api/syscall-ret>

namespace Alux {

anarch::SyscallRet CreatePortSyscall();
anarch::SyscallRet DestroyPortSyscall(anarch::SyscallArgs &);

}

#endif