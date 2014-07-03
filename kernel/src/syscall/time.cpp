#include <syscall/time.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <scheduler/general/hold-scope.hpp>
#include <scheduler/general/task.hpp>
#include <scheduler/general/thread.hpp>
#include <arch/general/clock.hpp>
#include <utilities/frac.hpp>
#include <critical>
#include <lock>

namespace OS {

ReturnValue SyscallGetBootMicroTime() {
  HoldScope scope;
  return ReturnValue::NewUInt64(Clock::GetClock().GetMicroTime());
}

void SyscallSetTimeout(ArgList & args) {
  HoldScope scope;
  
  uint64_t timeout = args.PopUInt64();
  
  Thread * thread = scope.GetThread();
  LockHold(&thread->timeoutLock);
  if (thread->shouldClearTimeout) {
    thread->shouldClearTimeout = false;
    LockRelease(&thread->timeoutLock);
    return;
  }
  thread->isSleeping = true;
  
  ScopeCritical critical;
  Frac64 frac(Clock::GetClock().GetTicksPerMin(), 60000000L);
  Scheduler::GetGlobal().SetTimeout(frac.Multiply(timeout),
                                    true, &thread->timeoutLock);
}

void SyscallSetInfiniteTimeout() {
  HoldScope scope;
  Thread * thread = scope.GetThread();
  LockHold(&thread->timeoutLock);
  if (thread->shouldClearTimeout) {
    thread->shouldClearTimeout = false;
    LockRelease(&thread->timeoutLock);
    return;
  }
  thread->isSleeping = true;
  
  ScopeCritical critical;
  Scheduler::GetGlobal().SetInfiniteTimeout(&thread->timeoutLock);
}

void SyscallClearTimeout(ArgList & args) {
  HoldScope scope;
  uint64_t threadId = args.PopUInt64();
  scope.GetTask()->UnsleepThreadById(threadId);
}

void SyscallClearClear() {
  HoldScope scope;
  
  Thread * thread = scope.GetThread();
  
  ScopeLock lock(&thread->timeoutLock);
  thread->shouldClearTimeout = false;
}

}