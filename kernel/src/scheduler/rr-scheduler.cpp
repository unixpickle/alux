#include "rr-scheduler.hpp"
#include <anarch/api/panic>
#include <anarch/api/clock>
#include <anarch/api/timer>
#include <anarch/api/thread>
#include <anarch/api/global-map>
#include <anarch/api/domain-list>
#include <anarch/api/clock-module>
#include <anarch/critical>

namespace Alux {

RRScheduler::RRScheduler() : collector(*this) {
  collectorTask = KernelTask::New(*this);
  if (!collectorTask) {
    anarch::Panic("RRScheduler() - failed to allocate collector task");
  }
  anarch::State & state = anarch::State::NewKernel(RunGarbageThread,
                                                   (void *)&collector);
  collectorThread = Thread::New(*collectorTask, state);
  if (!collectorThread) {
    anarch::Panic("RRScheduler() - failed to allocate collector thread");
  }
  Add(*collectorThread);
  collectorThread->Release();
  collectorTask->Unhold();
}

void RRScheduler::Add(Thread & t) {
  ThreadObj * obj = new ThreadObj(t);
  assert(obj != NULL);
  ThreadUserInfo(t) = (void *)obj;
  
  anarch::ScopedCritical critical;
  anarch::ScopedLock scope(lock);
  threads.Add(&obj->link);
}

void RRScheduler::Remove(Thread & t) {
  ThreadObj * obj = (ThreadObj *)ThreadUserInfo(t);
  {
    anarch::ScopedCritical critical;
    anarch::ScopedLock scope(lock);
    threads.Remove(&obj->link);
  }
  delete obj;
}

void RRScheduler::SetTimeout(uint64_t deadline) {
  anarch::ScopedCritical critical;
  Thread * th = Thread::GetCurrent();
  assert(th != NULL);
  ThreadObj * obj = (ThreadObj *)ThreadUserInfo(*th);
  obj->deadline = deadline;
  Yield();
}

void RRScheduler::SetTimeout(uint64_t deadline, ansa::Lock & unlock) {
  anarch::ScopedCritical critical;
  Thread * th = Thread::GetCurrent();
  assert(th != NULL);
  ThreadObj * obj = (ThreadObj *)ThreadUserInfo(*th);
  obj->deadline = deadline;
  unlock.Release();
  Yield();
}

void RRScheduler::SetInfiniteTimeout() {
  SetTimeout(0xffffffffffffffff);
}

void RRScheduler::SetInfiniteTimeout(ansa::Lock & unlock) {
  SetTimeout(0xffffffffffffffff, unlock);
}

void RRScheduler::ClearTimeout(Thread & th) {
  ThreadObj * obj = (ThreadObj *)ThreadUserInfo(th);
  obj->deadline = 0;
}

void RRScheduler::Yield() {
  anarch::ScopedCritical critical;
  Thread * th = Thread::GetCurrent();
  assert(th != NULL);
  th->GetState().SuspendAndCall(CallSwitch, (void *)this);
}

GarbageCollector & RRScheduler::GetGarbageCollector() { 
  return collector;
}

void RRScheduler::Run() {
  anarch::ScopedCritical critical;
  anarch::DomainList & domains = anarch::DomainList::GetGlobal();
  for (int i = 0; i < domains.GetCount(); ++i) {
    anarch::Domain & domain = domains[i];
    for (int j = 0; j < domain.GetThreadCount(); ++j) {
      anarch::Thread & thread = domain.GetThread(j);
      if (&thread == &anarch::Thread::GetCurrent()) continue;
      thread.RunAsync(CallSwitch, (void *)this);
    }
  }
  Switch();
}

void RRScheduler::SetGarbageTimeout(ansa::Lock & unlock) {
  assert(Thread::GetCurrent() == collectorThread);
  SetInfiniteTimeout(unlock);
}

void RRScheduler::ClearGarbageTimeout() {
  ClearTimeout(*collectorThread);
}

void RRScheduler::Switch() {
  AssertCritical();
  anarch::GlobalMap::GetGlobal().Set();
  uint64_t now = anarch::ClockModule::GetGlobal().GetClock().GetTicks();
  
  lock.Seize();
  ResignCurrent();
  
  Thread * nextThread = NULL;
  for (auto iter = threads.GetStart(); iter != threads.GetEnd(); ++iter) {
    ThreadObj & obj = *iter;
    Thread & thread = obj.thread;
    if (!thread.Retain()) continue;
    if (obj.deadline > now || obj.running) {
      thread.Release();
      continue;
    }
    obj.running = true;
    nextThread = &thread;
    break;
  }
  lock.Release();
  
  // compute the timeout info
  anarch::Timer & timer = anarch::Thread::GetCurrent().GetTimer();
  uint64_t timeout = timer.GetTicksPerMicro().ScaleInteger(JiffyUs);
  
  // switch to the thread or wait until the next timer iteration
  if (nextThread) {
    Thread::SetCurrent(nextThread);
    nextThread->GetTask().GetMemoryMap().Set();
    timer.SetTimeout(timeout, SuspendAndSwitch, (void *)this);
    nextThread->GetState().Resume();
  } else {
    timer.SetTimeout(timeout, RunSyncAndSwitch, (void *)this);
    timer.WaitTimeout();
  }
}

void RRScheduler::ResignCurrent() {
  Thread * th = Thread::GetCurrent();
  if (!th) return;
  
  Thread::SetCurrent(NULL);
  
  // move it to the back
  ThreadObj * obj = (ThreadObj *)ThreadUserInfo(*th);
  threads.Remove(&obj->link);
  threads.Add(&obj->link);
  
  // stop it and release it
  obj->running = false;
  th->Release();
}

void RRScheduler::CallSwitch(void * scheduler) {
  ((RRScheduler *)scheduler)->Switch();
}

void RRScheduler::SuspendAndSwitch(void * scheduler) {
  Thread * thread = Thread::GetCurrent();
  assert(thread != NULL);
  thread->GetState().SuspendAndCall(CallSwitch, scheduler);
}

void RRScheduler::RunSyncAndSwitch(void * scheduler) {
  anarch::Thread::RunSync(CallSwitch, scheduler);
}

void RRScheduler::RunGarbageThread(void * garbage) {
  ((GarbageCollector *)garbage)->Main();
}

}
