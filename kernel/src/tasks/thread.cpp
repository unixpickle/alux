#include "thread.hpp"
#include "task.hpp"
#include "../scheduler/scheduler.hpp"
#include <anarch/api/thread>
#include <anarch/critical>

namespace Alux {
  
Thread::~Thread() {
  state.Delete();
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

Thread * Thread::New(Task & t, anarch::State & s, Endpoint & e) {
  AssertNoncritical();
  if (!t.Retain()) return NULL;
  
  // create thread
  Thread * th = new Thread(t, s);
  if (!th) {
    t.Release();
    return NULL;
  }
  
  // create port
  UserPort * p = UserPort::New(*th);
  if (!p) {
    t.Release();
    delete th;
    return NULL;
  }
  
  // create remote endpoint
  Endpoint * ep = p->Open();
  if (!ep) {
    t.Release();
    th->DestroyPorts(0);
    delete th;
    return NULL;
  }
  
  if (!th->Init()) {
    t.Release();
    th->DestroyPorts(0);
    delete th;
  }
  
  // connect remote endpoint
  ep->Connect(e);
  return th;
}

Task & Thread::GetTask() const {
  return task;
}

anarch::State & Thread::GetState() const {
  return state;
}

SleepState & Thread::GetSleepState() {
  return sleepState;
}

PollState & Thread::GetPollState() {
  return pollState;
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
  Deinit();
  delete this;
}

Thread::Thread(Task & t, anarch::State & s)
  : GarbageObject(t.GetScheduler().GetGarbageCollector()), hashMapLink(*this),
    sleepState(*this), pollState(*this), task(t), state(s),
    ports(anidmap::IDENTIFIER_MAX) {
}

Thread::PortMap & Thread::GetPorts() {
  return ports;
}

bool Thread::Init() {
  if (!GetTask().GetThreads().Add(*this)) {
    return false;
  }
  GetTask().GetScheduler().Add(*this);
  return true;
}

void Thread::Deinit() {
  // close all ports belonging to the thread
  if (!killed) {
    DestroyPorts(1 + GetTask().GetKillReason());
  } else {
    DestroyPorts(0);
  }
  
  if (killed) {
    // if this thread was exited, we need to do some more cleanup
    GetTask().GetThreads().Remove(*this);
    GetTask().GetScheduler().Remove(*this);
    GetTask().Release();
  }
}

void Thread::DestroyPorts(int closeReason) {
  for (int i = 0; i < ports.GetMap().GetBucketCount(); ++i) {
    auto & bucket = ports.GetMap().GetBucket(i);
    for (auto iter = bucket.GetStart(); iter != bucket.GetEnd(); ++iter) {
      UserPort & p = *iter;
      p.Sever(closeReason);
      p.Delete(false);
    }
  }
}

}
