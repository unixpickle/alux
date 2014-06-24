#ifndef __SYSCALL_TIME_HPP__
#define __SYSCALL_TIME_HPP__

#include <cstdint>

namespace OS {

uint64_t SyscallGetBootMicroTime();
void SyscallSetTimeout(uint64_t timeout);
void SyscallSetInfiniteTimeout();
void SyscallClearTimeout(uint64_t threadId);
void SyscallClearClear();

}

#endif
