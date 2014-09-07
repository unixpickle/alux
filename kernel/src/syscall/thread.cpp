#include "thread.hpp"
#include "errors.hpp"
#include "../threads/sleep-state.hpp"
#include "../threads/thread.hpp"
#include "../tasks/hold-scope.hpp"

namespace Alux {

void ExitThreadSyscall() {
  HoldScope scope;
  scope.ExitThread();
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
  
  // create the thread and start it up!
  scope.GetTask().Retain();
  Thread & th = Thread::New(scope.GetTask(), state);
  if (!th.AddToTask()) {
    th.Release();
    th.Dealloc();
    return anarch::SyscallRet::Error(SyscallErrorUnableToLaunch);
  }
  th.AddToScheduler();
  th.Release();
  
  return anarch::SyscallRet::Integer32((uint32_t)th.GetIdentifier());
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
  
  Thread * th = scope.GetTask().GetThreadList().Find(identifier);
  if (!th) {
    return anarch::SyscallRet::Error(SyscallErrorNoThread);
  }
  
  SleepState::Unsleep(*th);
  th->Release();
  return anarch::SyscallRet::Empty();
}

}