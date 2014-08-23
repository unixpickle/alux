#include "thread.hpp"
#include "task.hpp"
#include "../scheduler/scheduler.hpp"
#include <anarch/api/thread>
#include <anarch/critical>

namespace Alux {

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
  
  Thread * th = GetCurrent();
  assert(th != NULL);
  th->lifeLock.Seize();
  th->killed = true;
  th->lifeLock.Release();
  th->GetTask().GetScheduler().Yield();
}

Thread * Thread::New(Task & t, anarch::State & s) {
  AssertNoncritical();
  if (!t.Retain()) return NULL;
  Thread * th = new Thread(t, s);
  if (!th) {
    t.Release();
    return NULL;
  }
  if (!th->Init()) {
    t.Release();
    delete th;
    return NULL;
  }
  return th;
}

Task & Thread::GetTask() const {
  return task;
}

anarch::State & Thread::GetState() const {
  return state;
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

Identifier Thread::GetIdentifier() const {
  return identifier;
}

void Thread::Dealloc() {
  AssertNoncritical();
  Deinit();
  delete this;
}

Thread::Thread(Task & t, anarch::State & s)
  : GarbageObject(t.GetScheduler().GetGarbageCollector()), idMapLink(*this),
    task(t), state(s) {
}

bool Thread::Init() {
  if (!GetTask().GetThreads().Add(*this, identifier)) {
    return false;
  }
  GetTask().GetScheduler().Add(*this);
  return true;
}

void Thread::Deinit() {
  if (killed) {
    GetTask().GetThreads().Remove(*this);
    GetTask().Release();
  }
}

}
