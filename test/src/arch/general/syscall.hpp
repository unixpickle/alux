#ifndef __GENERAL_SYSCALL_HPP__
#define __GENERAL_SYSCALL_HPP__

#include <cstdint>

extern "C" {

enum SyscallNumber {
  SyscallNumberPrint,
  SyscallNumberLaunchThread,
  SyscallNumberFork,
  SyscallNumberExit,
  SyscallNumberThreadExit,
  SyscallNumberGetPID
};

struct NewThreadInfo {
  void * stackTop;
  void (* func)(void *);
  void * argument;
} __attribute__((packed));

uint64_t Syscall(uint16_t name, void * arg1, void * arg2, void * arg3,
                 uint64_t arg4, uint64_t arg5);
void ThreadEntry(NewThreadInfo * info);
void ThreadExit();

}

#endif
