#ifndef __GENERAL_SYSCALL_HPP__
#define __GENERAL_SYSCALL_HPP__

#include <cstdint>

extern "C" {

enum SyscallNumber {
  SyscallNumberPrint
};

void Syscall(uint16_t name, void * arg1, void * arg2, void * arg3, uint64_t arg4, uint64_t arg5);

}

#endif
