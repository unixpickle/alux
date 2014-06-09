#include <scheduler-specific/scheduler.hpp>
#include <arch/general/cpu.hpp>
#include <arch/general/scheduler.hpp>
#include <arch/general/clock.hpp>
#include <utilities/lock.hpp>
#include <utilities/critical.hpp>
#include <new>

namespace OS {

static Scheduler globalObj;

void Scheduler::Initialize() {
  new(&globalObj) Scheduler();
}

Scheduler & Scheduler::GetGlobal() {
  return globalObj;
}

void Scheduler::AddThread(Thread * t) {
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  PushThread(t);
}

void Scheduler::RemoveThread(Thread * t) {
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  UnlinkThread(t);
}

void Scheduler::SetTimeout(uint64_t deadline, bool) {
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  
  Thread * th = CPU::GetCurrent().GetThread();
  assert(th != NULL);
  th->userInfo.nextTick = deadline;
  SaveAndTick();
}

void Scheduler::SetInfiniteTimeout() {
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  
  Thread * th = CPU::GetCurrent().GetThread();
  assert(th != NULL);
  th->userInfo.nextTick = UINT64_MAX;
  SaveAndTick();
}

bool Scheduler::ClearTimeout(Thread *) {
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  
  Thread * th = CPU::GetCurrent().GetThread();
  assert(th != NULL);
  bool wasDelayed = th->userInfo.nextTick != 0;
  th->userInfo.nextTick = 0;
  return wasDelayed;
}

void Scheduler::Resign() {
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  SaveAndTick();
}

void Scheduler::Tick() {
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  
  Thread * running = CPU::GetCurrent().GetThread();
  if (running) {
    running->userInfo.isRunning = false;
    CPU::SetThread(NULL);
  }
  
  Clock & clock = Clock::GetGlobal();
  uint64_t now = clock.GetTime();
  uint64_t nextTick = now + clock.GetTicksPerMin() / Jiffy;
  
  Thread * first = PopThread();
  Thread * current = first;
  
  while (current) {
    if (current != first) {
      PushThread(current);
      current = PopThread();
      if (current == first) break;
    }
    if (current->userInfo.isRunning) continue;
    if (current->userInfo.nextTick > now) {
      if (current->userInfo.nextTick < nextTick) {
        nextTick = current->userInfo.nextTick;
      }
      continue;
    }
    break;
  }
  
  SetTimeout(nextTick - now, false);
  if (!current) {
    WaitTimeout();
  } else {
    CPU::SetThread(current);
    current->userInfo.nextTick = 0;
    current->userInfo.isRunning = true;
    current->Run();
  }
}

// PRIVATE //

Thread * Scheduler::PopThread() {
  Thread * result = firstThread;
  if (!result) return NULL;
  if (result->userInfo.next) {
    result->userInfo.next->userInfo.last = result->userInfo.last;
  } else {
    lastThread = result->userInfo.last;
  }
  if (result->userInfo.last) {
    result->userInfo.last->userInfo.next = result->userInfo.next;
  } else {
    firstThread = result->userInfo.next;
  }
  result->userInfo.next = result->userInfo.last = NULL;
  return result;
}

void Scheduler::PushThread(Thread * th) {
  if (lastThread) {
    th->userInfo.last = lastThread;
    th->userInfo.next = NULL;
    lastThread->userInfo.next = th;
    lastThread = th;
  } else {
    lastThread = firstThread = th;
    th->userInfo.next = th->userInfo.last = NULL;
  }
}

void Scheduler::UnlinkThread(Thread * th) {
  if (!th->userInfo.next && !th->userInfo.last && th != firstThread) {
    return;
  }
  if (th->userInfo.next) {
    th->userInfo.next->userInfo.last = th->userInfo.last;
  } else {
    lastThread = th->userInfo.last;
  }
  if (th->userInfo.last) {
    th->userInfo.last->userInfo.next = th->userInfo.next;
  } else {
    firstThread = th->userInfo.next;
  }
  th->userInfo.next = th->userInfo.last = NULL;
}

}
