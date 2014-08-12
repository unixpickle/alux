#include "thread.hpp"
#include "task.hpp"
#include <anarch/api/thread>
#include <anarch/critical>
#include <anarch/new>
#include <ansa/atomic>

namespace OS {

namespace {

ansa::Atomic<uint64_t> counter;

}

void Thread::InitializeCounter() {
  new(&counter) ansa::Atomic<uint64_t>(0);
}

uint64_t Thread::ReadCounter() {
  return counter;
}

Thread * Thread::GetCurrent() {
  return (Thread *)anarch::Thread::GetUserInfo();
}

void Thread::SetCurrent(Thread * th) {
  anarch::Thread::SetUserInfo((void *)th);
}

static Thread * New(Task & t, anarch::State & s) {
  // TODO: here, use a pool
  return *(new Thread(t, s));
}

Task & Thread::GetTask() {
  return task;
}

anarch::State & Thread::GetState() {
  return state;
}

bool Thread::Retain() {
  anarch::ScopedCritical critical;
  anarch::ScopedLock lock(retainLock);
  if (isKilled) return false;
  if (!GetTask().Retain()) return false;
  ++retainCount;
  return true;
}

void Thread::Release() {
  anarch::ScopedCritical critical;
  Task & t = GetTask();
  
  retainLock.Seize();
  assert(retainCount != 0);
  bool shouldCleanup = !--retainCount && isKilled;
  retainLock.Release();
  
  // after releasing the lock, we may have been freed
  __asm__ __volatile__("" : : : "memory");
  
  if (shouldCleanup) {
    // this was the last Release() call; goodbye, cruel world.
    ThrowAway();
  } else {
    // If we get a Release() call and it's not the last one, but then another
    // Release() call comes in and it *is* the last one, we may have just been
    // deallocated before we reached this point. Thus, we can't use an instance
    // variable if we weren't the last Release(). However, we may use the stack
    // because a thread's retain count will never reach zero while it's
    // running.
    t.Release();
  }
}

ThreadId Thread::GetId() const {
  return identifier;
}

Thread::Thread(Task & t, anarch::State & s)
  : GarbageObject(t.GetScheduler().GetGarbageCollector()), task(t), state(s),
    schedulerLink(*this) {
  ++counter;
  identifier = GetTask().AddThread(*this);
}

Thread::~Thread() {
  --counter;
  state.Delete();
}

void Thread::Destroy() {
  AssertNoncritical();
  
  // only remove ourselves from the owning task if the owning task is still
  // retained
  retainLock.Seize();
  bool killed = retainCount != 0;
  retainLock.Release();
  if (killed) {
    GetTask().RemoveThread(*this);
    GetTask().Release();
  }
  // TODO: here, use a pool
  delete this;
}

}
