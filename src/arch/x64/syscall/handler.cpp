#include <arch/x64/syscall/handler.hpp>
#include <arch/general/hardware-thread.hpp>
#include <scheduler/general/hold-scope.hpp>
#include <critical>
#include <iostream>

extern "C" {

void SyscallMain(uint64_t call, uint64_t arg1,
                 uint64_t arg2, uint64_t arg3,
                 uint64_t arg4, uint64_t arg5) {
  OS::x64::HandleSyscall(call, arg1, arg2, arg3, arg4, arg5);
}

void _SyscallMain(uint64_t call, uint64_t arg1,
                  uint64_t arg2, uint64_t arg3,
                  uint64_t arg4, uint64_t arg5) {
  OS::x64::HandleSyscall(call, arg1, arg2, arg3, arg4, arg5);
}

}

namespace OS {

namespace x64 {

void HandleSyscall(uint64_t call, uint64_t arg1,
                   uint64_t arg2, uint64_t arg3,
                   uint64_t arg4, uint64_t arg5) {
  AssertCritical();
  
  HoldScope scope(HardwareThread::GetCurrent().GetThread()->GetTask());
  if (!scope.DidHold()) return;
  
  SetCritical(false);
  cout << "HandleSyscall(" << call << "," << arg1 << "," << arg2 << "," << arg3
    << "," << arg4 << "," << arg5 << ")" << endl;
  SetCritical(true);
}

}

}
