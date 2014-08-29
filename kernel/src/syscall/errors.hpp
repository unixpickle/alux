#ifndef __SYSCALL_ERRORS_HPP__
#define __SYSCALL_ERRORS_HPP__

namespace Alux {

enum {
  SyscallErrorPermissions = 1,
  SyscallErrorIndex,
  SyscallErrorNoMemory,
  SyscallErrorNoMapping,
  SyscallErrorNoVMSpace,
  SyscallErrorUnableToLaunch,
  SyscallErrorNoThread
};

}

#endif
