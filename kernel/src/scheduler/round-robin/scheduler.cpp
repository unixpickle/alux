#include "scheduler.hpp"

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
  // TODO: this
}

void RRScheduler::SetInfiniteTimeout(ansa::Lock & unlock) {
  // TODO: this
}

void RRScheduler::ClearTimeout(Thread & thread) {
  // TODO: this
}

void RRScheduler::Yield() {
  // TODO: this
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

}
