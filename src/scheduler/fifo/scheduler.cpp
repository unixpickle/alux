#include <scheduler-specific/scheduler.hpp>
#include <arch/general/hardware-thread-list.hpp>
#include <arch/general/scheduler-expert.hpp>
#include <arch/general/clock.hpp>
#include <utilities/lock.hpp>
#include <utilities/critical.hpp>
#include <new>

#include <iostream> // TODO: delete this
#include <arch/x64/general/critical.hpp> // TODO: delete this
#include <arch/general/failure.hpp> // TODO: delete this

namespace OS {

static Scheduler globalObj;

static void LoopString(const char * string);

void Scheduler::Initialize() {
  new(&globalObj) Scheduler();
}

Scheduler & Scheduler::GetGlobal() {
  return globalObj;
}

void Scheduler::Start() {
  AssertNoncritical();
  
  SchedulerExpert & expert = SchedulerExpert::GetGlobal();
  Task * t = expert.CreateKernelTask();
  Thread * th1 = expert.CreateKernelThread(t, (void *)&LoopString,
                                           (void *)"tick");
  Thread * th2 = expert.CreateKernelThread(t, (void *)&LoopString,
                                           (void *)"tock");
  
  SetCritical(true);
  AddThread(th1);
  AddThread(th2);
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
    th->userInfo.nextTick = deadline;
  }
  SchedulerExpert::GetGlobal().SaveAndTick();
}

void Scheduler::SetInfiniteTimeout() {
  AssertCritical();
  {
    ScopeCriticalLock scope(&lock);
  
    Thread * th = HardwareThread::GetCurrent().GetThread();
    assert(th != NULL);
    th->userInfo.nextTick = UINT64_MAX;
  }
  SchedulerExpert::GetGlobal().SaveAndTick();
}

bool Scheduler::ClearTimeout(Thread *) {
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  
  Thread * th = HardwareThread::GetCurrent().GetThread();
  assert(th != NULL);
  bool wasDelayed = th->userInfo.nextTick != 0;
  th->userInfo.nextTick = 0;
  return wasDelayed;
}

void Scheduler::Resign() {
  AssertCritical();
  SchedulerExpert::GetGlobal().SaveAndTick();
}

void Scheduler::Tick() {
  int index = HardwareThread::GetCurrent().GetIndex();
  AssertCritical();
  
  Thread * toRun = GetNextThread();
  if (!toRun) {
    SchedulerExpert::GetGlobal().WaitTimeout();
  } else {
    toRun->Run();
  }
}

// PRIVATE //

Thread * Scheduler::GetNextThread() {
  ScopeCriticalLock scope(&lock);
  
  Thread * running = HardwareThread::GetCurrent().GetThread();
  if (running) {
    running->userInfo.isRunning = false;
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
      
    if (current->userInfo.isRunning) continue;
    if (current->userInfo.nextTick > now) {
      if (current->userInfo.nextTick < nextTick) {
        nextTick = current->userInfo.nextTick;
      }
      continue;
    }
    break;
  }
  
  if (current) {
    current->userInfo.isRunning = true;
    current->userInfo.nextTick = 0;
    HardwareThread::SetThread(current);
  }
  SchedulerExpert::GetGlobal().SetTimeout(nextTick - now, true);
  return current;
}

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

static void LoopString(const char * string) {
  while (1) {
    uint64_t waitUntil = Clock::GetGlobal().GetTime()
      + Clock::GetGlobal().GetTicksPerMin() / 120;
    SetCritical(true);
    Scheduler::GetGlobal().SetTimeout(waitUntil, true);
    SetCritical(false);
    cout << string << endl;
  }
}

}
