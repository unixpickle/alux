#ifndef __SYSCALL_ERRORS_HPP__
#define __SYSCALL_ERRORS_HPP__

namespace Alux {

enum {
  SyscallErrorIndex = 1,
  SyscallErrorPermissions,
  SyscallErrorNoMemory,
  SyscallErrorNoMapping
};

}

#endif
