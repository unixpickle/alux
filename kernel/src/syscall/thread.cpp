#include "thread.hpp"
#include "errors.hpp"
#include "../tasks/sleep-state.hpp"
#include "../tasks/hold-scope.hpp"
#include "../tasks/thread.hpp"

namespace Alux {

void ExitThreadSyscall() {
  HoldScope scope;
  if (scope.GetTask().GetThreads().GetCount() == 1) {
    // we are the last thread
    scope.ExitTask(Task::KillReasonNormal);
  } else {
    scope.ExitThread();
  }
}

anarch::SyscallRet GetThreadIdSyscall() {
  HoldScope scope;
  uint32_t ident = (uint32_t)scope.GetThread().GetIdentifier();
  return anarch::SyscallRet::Integer32(ident);
}

anarch::SyscallRet LaunchThreadSyscall(anarch::SyscallArgs & args) {
  HoldScope scope;
  VirtAddr callAddress = args.PopVirtAddr();
  VirtAddr argument = args.PopVirtAddr();
  anarch::State & state = anarch::State::NewUser((void (*)(void *))callAddress,
                                                 (void *)argument);
  Thread * th = Thread::New(scope.GetTask(), state);
  if (!th) {
    state.Delete();
    return anarch::SyscallRet::Error(SyscallErrorUnableToLaunch);
  }
  return anarch::SyscallRet::Integer32((uint32_t)th->GetIdentifier());
}

void SleepSyscall(anarch::SyscallArgs & args) {
  uint64_t duration = args.PopUInt64();
  SleepState::Sleep(duration);
}

void SleepInfiniteSyscall() {
  SleepState::SleepInfinite();
}

anarch::SyscallRet WakeupSyscall(anarch::SyscallArgs & args) {
  HoldScope scope;
  uint32_t identifier = args.PopUInt32();
  
  Thread * th = scope.GetTask().GetThreads().Find(identifier);
  if (!th) {
    return anarch::SyscallRet::Error(SyscallErrorNoThread);
  }
  
  th->GetSleepState().Cancel();
  return anarch::SyscallRet::Empty();
}

}