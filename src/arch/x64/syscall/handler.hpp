#ifndef __SYSCALL_HANDLER__
#define __SYSCALL_HANDLER__

#include <cstdint>

extern "C" {

void SyscallMain(uint64_t call, uint64_t arg1,
                 uint64_t arg2, uint64_t arg3,
                 uint64_t arg4, uint64_t arg5);
void _SyscallMain(uint64_t call, uint64_t arg1,
                  uint64_t arg2, uint64_t arg3,
                  uint64_t arg4, uint64_t arg5);

}

namespace OS {

namespace x64 {

void HandleSyscall(uint64_t call, uint64_t arg1,
                   uint64_t arg2, uint64_t arg3,
                   uint64_t arg4, uint64_t arg5);

}

}

#endif
