#include "thread.hpp"
#include "task.hpp"
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

Thread::Thread(Task & t, anarch::State & s) : task(t), state(s) {
  ++counter;
}

Thread::~Thread() {
  --counter;
  state.Delete();
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
  
  if (shouldCleanup) {
    // this was the last Release() call; goodbye, cruel world.
    PushGarbage();
  } else {
    // If we get a Release() call and it's not the last one, but then another
    // Release() call comes in and it *is* the last one, we may have just been
    // deallocated before we reached this point. Thus, we can't use an instance
    // variable if we weren't the last Release().
    t.Release();
  }
}

void Thread::PushGarbage() {
  // TODO: push ourselves to a garbage thread here
}

}
