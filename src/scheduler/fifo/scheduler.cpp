#include <scheduler-specific/scheduler.hpp>
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
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  PushThread(t);
}

void Scheduler::RemoveThread(Thread * t) {
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  UnlinkThread(t);
}

void Scheduler::SetTimeout(uint64_t deadline, bool, uint64_t * unlock) {
  AssertCritical();
  {
    ScopeCriticalLock scope(&lock);

    Thread * th = HardwareThread::GetThread();
    assert(th != NULL);
    th->SchedThread::nextTick = deadline;
    if (unlock) LockRelease(unlock);
  }
  TickTimer::GetGlobal().SaveAndTick();
}

void Scheduler::SetInfiniteTimeout(uint64_t * unlock) {
  AssertCritical();
  {
    ScopeCriticalLock scope(&lock);
  
    Thread * th = HardwareThread::GetThread();
    assert(th != NULL);
    th->SchedThread::nextTick = UINT64_MAX;
    if (unlock) LockRelease(unlock);
  }
  TickTimer::GetGlobal().SaveAndTick();
}

bool Scheduler::ClearTimeout(Thread *) {
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  
  Thread * th = HardwareThread::GetThread();
  assert(th != NULL);
  bool wasDelayed = th->SchedThread::nextTick != 0;
  th->SchedThread::nextTick = 0;
  return wasDelayed;
}

void Scheduler::Resign() {
  AssertCritical();
  TickTimer::GetGlobal().SaveAndTick();
}

void Scheduler::ExitThread() {
  AssertCritical();
  Thread * th = HardwareThread::GetThread();
  TickTimer::GetGlobal().TickAtMethod(&Scheduler::TerminateThread,
                                      (void *)th);
}

void Scheduler::ExitTask(uint64_t status) {
  AssertCritical();
  Thread * th = HardwareThread::GetThread();
  assert(th != NULL);
  Task * task = th->GetTask();
  task->Kill(status);
  ExitThread();
}

void Scheduler::Tick() {
  AssertCritical();
  
  Thread * toRun = GetNextThread();
  if (!toRun) {
    TickTimer::GetGlobal().WaitTimeout();
  } else {
    toRun->GetState().Load();
  }
}

// PRIVATE //

void Scheduler::SwitchThread(Thread * t) {
  if (t) {
    t->GetTask()->GetAddressSpace().Set();
  } else {
    GlobalMap::GetGlobal().Set();
  }
  
  Thread * running = HardwareThread::GetThread();
  if (running) {
    HardwareThread::SetThread(NULL);
    running->SchedThread::isRunning = false;
    running->GetTask()->Release();
  }
  
  if (t) {
    t->SchedThread::isRunning = true;
    t->SchedThread::nextTick = 0;
    HardwareThread::SetThread(t);
  }
}

void Scheduler::TerminateThread(void * thPtr) {
  Thread * thread = (Thread *)thPtr;
  Task * task = thread->GetTask();
  if (!task->Retain()) {
    // the task is dying anyway
    return Scheduler::GetGlobal().Tick();
  }
  Scheduler::GetGlobal().RemoveThread(thread); // never run it again
  SwitchThread(NULL); // release the task, unset it from the CPU
  task->RemoveThread(thread); // remove the task's reference to the thread
  GarbageThread::GetGlobal().PushThread(thread);
  task->Release(); // release our reference
}

Thread * Scheduler::GetNextThread() {
  ScopeCriticalLock scope(&lock);
  
  Clock & clock = Clock::GetClock();
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
      
    if (current->SchedThread::isRunning) continue;
    if (current->SchedThread::nextTick > now) {
      if (current->SchedThread::nextTick < nextTick) {
        nextTick = current->SchedThread::nextTick;
      }
      continue;
    }
    
    if (!current->GetTask()->Retain()) continue;
    break;
  }
  
  SwitchThread(current);
  TickTimer::GetGlobal().SetTimeout(nextTick - now, true);
  return current;
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
