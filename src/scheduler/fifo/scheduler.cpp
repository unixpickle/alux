#include <scheduler-specific/scheduler.hpp>
#include <arch/general/hardware-thread-list.hpp>
#include <arch/general/tick-timer.hpp>
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

void Scheduler::Start() {
  AssertNoncritical();
  SetCritical(true);
  HardwareThreadList & list = HardwareThreadList::GetGlobal();
  HardwareThread & thisCPU = HardwareThread::GetCurrent();
  for (int i = 0; i < list.GetCount(); i++) {
    if (&list[i] == &thisCPU) continue;
    list[i].Wake();
  }
  Tick();
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
  {
    ScopeCriticalLock scope(&lock);

    Thread * th = HardwareThread::GetCurrent().GetThread();
    assert(th != NULL);
    th->SchedulerThread::nextTick = deadline;
  }
  TickTimer::GetCurrent().SaveAndTick();
}

void Scheduler::SetInfiniteTimeout() {
  AssertCritical();
  {
    ScopeCriticalLock scope(&lock);
  
    Thread * th = HardwareThread::GetCurrent().GetThread();
    assert(th != NULL);
    th->SchedulerThread::nextTick = UINT64_MAX;
  }
  TickTimer::GetCurrent().SaveAndTick();
}

bool Scheduler::ClearTimeout(Thread *) {
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  
  Thread * th = HardwareThread::GetCurrent().GetThread();
  assert(th != NULL);
  bool wasDelayed = th->SchedulerThread::nextTick != 0;
  th->SchedulerThread::nextTick = 0;
  return wasDelayed;
}

void Scheduler::Resign() {
  AssertCritical();
  TickTimer::GetCurrent().SaveAndTick();
}

void Scheduler::Tick() {
  int index = HardwareThread::GetCurrent().GetIndex();
  AssertCritical();
  
  Thread * toRun = GetNextThread();
  if (!toRun) {
    TickTimer::GetCurrent().WaitTimeout();
  } else {
    toRun->Run();
  }
}

// PRIVATE //

Thread * Scheduler::GetNextThread() {
  ScopeCriticalLock scope(&lock);
  
  Thread * running = HardwareThread::GetCurrent().GetThread();
  if (running) {
    running->SchedulerThread::isRunning = false;
    HardwareThread::SetThread(NULL);
  }
  
  Clock & clock = Clock::GetGlobal();
  uint64_t now = clock.GetTime();
  uint64_t nextTick = now + clock.GetTicksPerMin() / Jiffy;
  
  Thread * first = NULL;
  Thread * current = NULL;
  
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
      
    if (current->SchedulerThread::isRunning) continue;
    if (current->SchedulerThread::nextTick > now) {
      if (current->SchedulerThread::nextTick < nextTick) {
        nextTick = current->SchedulerThread::nextTick;
      }
      continue;
    }
    
    if (!current->GetTask()->Retain()) continue;
    break;
  }
  
  if (current) {
    current->SchedulerThread::isRunning = true;
    current->SchedulerThread::nextTick = 0;
    HardwareThread::SetThread(current);
  }
  TickTimer::GetCurrent().SetTimeout(nextTick - now, true);
  return current;
}

Thread * Scheduler::PopThread() {
  Thread * result = firstThread;
  if (!result) return NULL;
  if (result->SchedulerThread::next) {
    result->SchedulerThread::next->SchedulerThread::last
      = result->SchedulerThread::last;
  } else {
    lastThread = result->SchedulerThread::last;
  }
  if (result->SchedulerThread::last) {
    result->SchedulerThread::last->SchedulerThread::next
      = result->SchedulerThread::next;
  } else {
    firstThread = result->SchedulerThread::next;
  }
  result->SchedulerThread::next = result->SchedulerThread::last = NULL;
  return result;
}

void Scheduler::PushThread(Thread * th) {
  if (lastThread) {
    th->SchedulerThread::last = lastThread;
    th->SchedulerThread::next = NULL;
    lastThread->SchedulerThread::next = th;
    lastThread = th;
  } else {
    lastThread = firstThread = th;
    th->SchedulerThread::next = th->SchedulerThread::last = NULL;
  }
}

void Scheduler::UnlinkThread(Thread * th) {
  if (!th->SchedulerThread::next && !th->SchedulerThread::last
      && th != firstThread) {
    return;
  }
  if (th->SchedulerThread::next) {
    th->SchedulerThread::next->SchedulerThread::last
      = th->SchedulerThread::last;
  } else {
    lastThread = th->SchedulerThread::last;
  }
  if (th->SchedulerThread::last) {
    th->SchedulerThread::last->SchedulerThread::next
      = th->SchedulerThread::next;
  } else {
    firstThread = th->SchedulerThread::next;
  }
  th->SchedulerThread::next = th->SchedulerThread::last = NULL;
}

}
