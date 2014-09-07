// no need to include "thread.hpp" or "task.hpp" ourselves
#include "../scheduler/scheduler.hpp"
#include <anarch/api/thread>
#include <anarch/critical>

namespace Alux {
  
Thread::~Thread() {
  state.Delete();
}

Thread & Thread::New(Task & t, anarch::State & s) {
  AssertNoncritical();
  Thread * result = new Thread(t, s);
  assert(result != NULL);
  return *result;
}

Thread * Thread::GetCurrent() {
  anarch::ScopedCritical critical;
  return (Thread *)anarch::Thread::GetUserInfo();
}

void Thread::SetCurrent(Thread * th) {
  AssertCritical();
  anarch::Thread::SetUserInfo((void *)th);
}

void Thread::ExitCurrent() {
  // no point creating a ScopedCritical; it would never be destroyed
  anarch::SetCritical(true);
  
  // since this thread is already running, it must be retained
  Thread * th = GetCurrent();
  th->Kill();
  
  // the scheduler will never be able to retain this thread again, so this will
  // be the last line of code ever run on this thread
  th->GetTask().GetScheduler().Yield();
  
  __builtin_unreachable();
}

Thread::Thread(Task & t, anarch::State & s)
  : GarbageObject(t.GetScheduler().GetGarbageCollector()), hashMapLink(*this),
    sleepState(*this), task(t), state(s) {
}

bool Thread::AddToTask() {
  if (!task.GetThreadList().Add(*this)) {
    return false;
  }
  return inTask = true;
}

void Thread::AddToScheduler() {
  assert(inTask);
  task.GetScheduler().Add(*this);
  inScheduler = true;
}

bool Thread::Retain() {
  anarch::ScopedCritical critical;
  anarch::ScopedLock scope(lifeLock);
  if (killed) return false;
  if (!GetTask().Retain()) return false;
  ++retainCount;
  return true;
}

void Thread::Release() {
  anarch::ScopedCritical critical;
  anarch::ScopedLock scope(lifeLock);
  --retainCount;
  if (!retainCount && killed) {
    ThrowAway();
  } else {
    GetTask().Release();
  }
}

void Thread::Kill() {
  anarch::ScopedCritical critical;
  anarch::ScopedLock scope(lifeLock);
  killed = true;
}

void Thread::Dealloc() {
  AssertNoncritical();
  if (killed) {
    if (inTask) {
      GetTask().GetThreadList().Remove(*this);
    }
    GetTask().Release();
  }
  if (inScheduler) {
    GetTask().GetScheduler().Remove(*this);
  }
  delete this;
}

}
