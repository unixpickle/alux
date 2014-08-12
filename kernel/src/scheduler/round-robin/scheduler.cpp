#include "scheduler.hpp"
#include "../current.hpp"
#include <anarch/api/global-map>

namespace OS {

void RRScheduler::Init() {
  anarch::State & state = anarch::State::NewKernel(StartGarbageThread,
                                                   (void *)this);
  kernelTask = &KernelTask::New(*this);
  garbageThread = &Thread::New(*kernelTask, state);
  garbageCollector = new GarbageCollector(*garbageThread);
}

void RRScheduler::Add(Thread & th) {
  AssertNoncritical();
  
  // TODO: allocate ThreadInfo from a slab
  ThreadInfo * info = new ThreadInfo();
  th->schedulerUserInfo = (void *)info;
  
  anarch::ScopedCritical critical;
  anarch::ScopedLock scope(threadsLock);
  threads.Add(&th.schedulerLink);
  
  th.isScheduled = true;
}

void RRScheduler::Remove(Thread & th) {
  AssertNoncritical();
  
  th.isScheduled = false;
  
  // TODO: free ThreadInfo from a slab
  ThreadInfo * info = (ThreadInfo *)th.schedulerUserInfo
  delete info;
  
  anarch::ScopedCritical critical;
  anarch::ScopedLock scope(lock);
  threads.Remove(&th.schedulerLock);
}

void RRScheduler::SetTimeout(uint64_t deadline, ansa::Lock & unlock) {
  ansa::ScopedCritical critical;
  
  Thread * thread = GetCurrentThread();
  assert(thread != NULL);
  ThreadInfo * info = (ThreadInfo *)thread.schedulerUserInfo;
  
  info->nextTick = deadline;
  unlock.Release();
  
  // run a tick from the kernel state
  thread->GetState().SuspendAndCall(CallSwich, (void *)this);
}

void RRScheduler::SetInfiniteTimeout(ansa::Lock & unlock) {
  ansa::ScopedCritical critical;
  
  Thread * thread = GetCurrentThread();
  assert(thread != NULL);
  ThreadInfo * info = (ThreadInfo *)thread.schedulerUserInfo;
  
  info->nextTick = UINT64_MAX;
  unlock.Release();
  
  // run a tick from the kernel state
  thread->GetState().SuspendAndCall(CallSwich, (void *)this);
}

void RRScheduler::ClearTimeout(Thread & thread) {
  // we don't really *need* this to be critical
  ((ThreadInfo *)thread.schedulerUserInfo)->nextTick = 0;
}

void RRScheduler::Yield() {
  ansa::ScopedCritical critical;
  thread->GetState().SuspendAndCall(CallSwich, (void *)this);
}

GarbageCollector & RRScheduler::GetGarbageCollector() const {
  return *garbageCollector;
}

TaskIdPool & RRScheduler::GetTaskIdPool() const {
  return pidPool;
}

void RRScheduler::StartGarbage(void * schedPtr) {
  RRScheduler & sched = *(RRScheduler *)schedPtr;
  sched.GetGarbageCollector().Main();
}

void RRScheduler::CallSwitch(void * obj) {
  RRScheduler & sched = *(RRScheduler *)obj;
  sched.Switch();
}

void RRScheduler::Switch() {
  AssertCritical();
  
  anarch::GlobalMap::GetGlobal().Set();
  uint64_t now = anarch::ClockModule::GetGlobal().GetClock().GetTicks();
  
  lock.Seize();
  Thread * th = Thread::GetCurrent():
  if (th) {
    // push the current thread to the back
    threads.Remove(&th.schedulerLink);
    threads.Push(&th.schedulerLink);
    Thread::SetCurrent(NULL);
  }
  Thread * nextThread = NULL;
  auto iter = threads.GetStart();
  while (iter != threads.GetEnd()) {
    Thread & thread = *(iter++);
    if (!thread.Retain()) continue;
    ThreadInfo & info = *(ThreadInfo *)thread.schedulerUserInfo;
    if (info.nextTick > now || info.isRunning) {
      thread.Release();
      continue;
    }
    info.isRunning = true;
    nextThread = &thread;
    break;
  }
  lock.Release();
  
  // compute timout info
  anarch::Timer & timer = anarch::Thread::GetCurrent().GetTimer();
  uint64_t timeout = timer.GetTicksPerMicro().Scale(JiffyUs);
  
  // switch to the thread
  if (nextThread) {
    nextThread->GetTask().GetMemoryMap().Set();
    timer.SetTimeout(timeout, CallSwitch, (void *)this);
    nextThread->GetState().Resume();
  } else {
    timer.SetTimeout(timeout, CallSwitch, (void *)this);
  }
}

}
