#include <scheduler-specific/scheduler.hpp>
#include <scheduler/fifo/contextualizer.hpp>
#include <scheduler/internal/garbage-thread.hpp>
#include <arch/general/hardware-thread-list.hpp>
#include <arch/general/tick-timer.hpp>
#include <arch/general/clock.hpp>
#include <arch/general/state.hpp>
#include <arch/general/global-map.hpp>
#include <critical>
#include <lock>
#include <new>

namespace OS {

static Scheduler globalObj;

void Scheduler::InitGlobal() {
  new(&globalObj) Scheduler();
}

Scheduler & Scheduler::GetGlobal() {
  return globalObj;
}

DepList Scheduler::GetDependencies() {
  return DepList(&HardwareThreadList::GetGlobal(), &TickTimer::GetGlobal(),
                 &ClockModule::GetGlobal());
}

void Scheduler::Start() {
  AssertNoncritical();
  SetCritical(true);
  HardwareThreadList & list = HardwareThreadList::GetGlobal();
  HardwareThread & thisCPU = HardwareThread::GetCurrent();
  for (int i = 0; i < list.GetCount(); i++) {
    if (&list[i] == &thisCPU) continue;
    list[i].Wake();
  }
  TickTimer::GetGlobal().SaveAndTick();
}

void Scheduler::AddThread(Thread * t) {
  ScopeCritical critical;
  ScopeCriticalLock scope(&lock);
  PushThread(t);
}

void Scheduler::SetTimeout(uint64_t deadline, bool, uint64_t * unlock) {
  ScopeCritical critical;
  {
    ScopeCriticalLock scope(&lock);

    Thread * th = HardwareThread::GetThread();
    assert(th != NULL);
    th->SchedThread::nextTick = deadline;
    if (unlock) LockRelease(unlock);
  }
  TickTimer::GetGlobal().ClearTimeout();
  TickTimer::GetGlobal().SaveAndTick();
}

void Scheduler::SetInfiniteTimeout(uint64_t * unlock) {
  ScopeCritical critical;
  {
    ScopeCriticalLock scope(&lock);
  
    Thread * th = HardwareThread::GetThread();
    assert(th != NULL);
    th->SchedThread::nextTick = UINT64_MAX;
    if (unlock) LockRelease(unlock);
  }
  TickTimer::GetGlobal().ClearTimeout();
  TickTimer::GetGlobal().SaveAndTick();
}

bool Scheduler::ClearTimeout(Thread * th) {
  ScopeCritical critical;
  ScopeCriticalLock scope(&lock);
  
  bool wasDelayed = th->SchedThread::nextTick != 0;
  th->SchedThread::nextTick = 0;
  return wasDelayed;
}

void Scheduler::Resign() {
  ScopeCritical critical;
  TickTimer::GetGlobal().SaveAndTick();
}

void Scheduler::Tick() {
  AssertCritical();
  
  uint64_t delay;
  Thread * toRun = GetNextThread(delay);
  TickTimer::GetGlobal().SetTimeout(delay, true);
  
  if (!toRun) {
    TickTimer::GetGlobal().WaitTimeout();
  } else {
    toRun->GetState().Load();
  }
}

// PROTECTED //

void Scheduler::RemoveThread(Thread * t) {
  ScopeCritical critical;
  ScopeCriticalLock scope(&lock);
  UnlinkThread(t);
}

Thread * Scheduler::GetNextThread(uint64_t & nextDelay) {
  AssertCritical();
  LockHold(&lock);
  
  Clock & clock = Clock::GetClock();
  uint64_t now = clock.GetTime();
  uint64_t nextTick = now + clock.GetTicksPerMin() / Jiffy;
  
  Thread * first = NULL;
  Thread * current = NULL;
  
  Contextualizer ctx;
  
  while (1) {
    if (first) {
      current = PopThread();
      PushThread(current);
      if (current == first) {
        current = NULL;
        break;
      }
    } else {
      first = PopThread();
      if (!first) break;
      current = first;
      PushThread(current);
    }
    
    if (current->SchedThread::nextTick > now) {
      if (current->SchedThread::nextTick < nextTick) {
        nextTick = current->SchedThread::nextTick;
      }
      continue;
    }
    
    if (ctx.SetNewThread(current)) break;
  }
  
  nextDelay = nextTick - now;
  
  ctx.SwitchRunningInfo();
  
  LockRelease(&lock);
  
  ctx.SwitchAddressSpace();
  ctx.ReleaseLast();
  
  return ctx.GetNewThread();
}

Thread * Scheduler::PopThread() {
  Thread * result = firstThread;
  if (!result) return NULL;
  if (result->SchedThread::next) {
    result->SchedThread::next->SchedThread::last = result->SchedThread::last;
  } else {
    lastThread = result->SchedThread::last;
  }
  if (result->SchedThread::last) {
    result->SchedThread::last->SchedThread::next = result->SchedThread::next;
  } else {
    firstThread = result->SchedThread::next;
  }
  result->SchedThread::next = result->SchedThread::last = NULL;
  return result;
}

void Scheduler::PushThread(Thread * th) {
  if (lastThread) {
    th->SchedThread::last = lastThread;
    th->SchedThread::next = NULL;
    lastThread->SchedThread::next = th;
    lastThread = th;
  } else {
    lastThread = firstThread = th;
    th->SchedThread::next = th->SchedThread::last = NULL;
  }
}

void Scheduler::UnlinkThread(Thread * th) {
  if (!th->SchedThread::next && !th->SchedThread::last
      && th != firstThread) {
    return;
  }
  if (th->SchedThread::next) {
    th->SchedThread::next->SchedThread::last = th->SchedThread::last;
  } else {
    lastThread = th->SchedThread::last;
  }
  if (th->SchedThread::last) {
    th->SchedThread::last->SchedThread::next = th->SchedThread::next;
  } else {
    firstThread = th->SchedThread::next;
  }
  th->SchedThread::next = th->SchedThread::last = NULL;
}

}
