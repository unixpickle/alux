#include <scheduler-specific/scheduler.hpp>
#include <scheduler/general/hold-scope.hpp>
#include <scheduler/general/task.hpp>
#include <scheduler/general/thread.hpp>
#include <arch/general/clock.hpp>
#include <utilities/frac.hpp>
#include <critical>
#include <lock>

namespace OS {

uint64_t SyscallGetBootMicroTime() {
  HoldScope scope;
  return Clock::GetClock().GetMicroTime();
}

void SyscallSetTimeout(uint64_t timeout) {
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

void SyscallClearTimeout(uint64_t threadId) {
  HoldScope scope;
  scope.GetTask()->UnsleepThreadById(threadId);
}

void SyscallClearClear() {
  HoldScope scope;
  
  Thread * thread = scope.GetThread();
  
  ScopeLock lock(&thread->timeoutLock);
  thread->shouldClearTimeout = false;
}

}