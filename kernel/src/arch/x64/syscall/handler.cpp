#include <arch/x64/syscall/handler.hpp>
#include <arch/x64/general/arg-list.hpp>
#include <arch/general/hardware-thread.hpp>
#include <scheduler/general/hold-scope.hpp>
#include <syscall/handler.hpp>
#include <critical>
#include <iostream>

extern "C" {

uint64_t SyscallMain(uint64_t call, uint64_t arg1,
                     uint64_t arg2, uint64_t arg3,
                     uint64_t arg4, uint64_t arg5) {
  return OS::x64::HandleSyscall(call, arg1, arg2, arg3, arg4, arg5);
}

uint64_t _SyscallMain(uint64_t call, uint64_t arg1,
                      uint64_t arg2, uint64_t arg3,
                      uint64_t arg4, uint64_t arg5) {
  return OS::x64::HandleSyscall(call, arg1, arg2, arg3, arg4, arg5);
}

}

namespace OS {

namespace x64 {

uint64_t HandleSyscall(uint64_t call, uint64_t arg1,
                       uint64_t arg2, uint64_t arg3,
                       uint64_t arg4, uint64_t arg5) {
  AssertCritical();
  
  // TODO: here, put architecture-specific system calls
  
  ArgList list(arg1, arg2, arg3, arg4, arg5);
  ReturnValue val = SyscallHandler((uint16_t)call, list);
  return val.value.unsigned64;
}

}

}
