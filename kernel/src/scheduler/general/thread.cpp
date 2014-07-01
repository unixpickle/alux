#include <scheduler/general/thread.hpp>
#include <scheduler/general/task.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <arch/general/hardware-thread.hpp>
#include <arch/general/state.hpp>

namespace OS {

static Atomic<uint64_t> threadCount(0);

Thread * Thread::NewUser(Task * owner, void * call) {
  {
    ScopeCritical critical;
    if (!owner->Retain()) return NULL;
  }
  return new Thread(owner, false, call);
}

Thread * Thread::NewUser(Task * owner, void * call, void * arg) {
  {
    ScopeCritical critical;
    if (!owner->Retain()) return NULL;
  }
  return new Thread(owner, false, call, arg);
}

Thread * Thread::NewKernel(Task * owner, void * call) {
  {
    ScopeCritical critical;
    if (!owner->Retain()) return NULL;
  }
  return new Thread(owner, true, call);
}

Thread * Thread::NewKernel(Task * owner, void * call, void * arg) {
  {
    ScopeCritical critical;
    if (!owner->Retain()) return NULL;
  }
  return new Thread(owner, true, call, arg);
}

void Thread::InitializeCounter() {
  new(&threadCount) Atomic<uint64_t>(0);
}

uint64_t Thread::GetCounter() {
  return threadCount;
}

void Thread::Exit() {
  AssertCritical();
  Thread * thread = HardwareThread::GetThread();
  assert(thread != NULL);
  {
    ScopeCriticalLock scope(&thread->retainLock);
    thread->isKilled = true;
    assert(thread->retainCount != 0);
  }
  Scheduler::GetGlobal().Resign();
  __builtin_unreachable();
}

Thread::~Thread() {
  state->Delete();
  --threadCount;
}

Thread::Thread(Task * owner, bool kernel, void * func) : task(owner) {
  ++threadCount;
  if (kernel) {
    state = State::NewKernel(func);
  } else {
    state = State::NewUser(func);
  }
}

Thread::Thread(Task * owner, bool kernel, void * func, void * arg)
  : task(owner) {
  ++threadCount;
  if (kernel) {
    state = State::NewKernel(func, arg);
  } else {
    state = State::NewUser(func, arg);
  }
}

void Thread::Delete() {
  delete this;
}

Task * Thread::GetTask() {
  return task;
}

State & Thread::GetState() {
  return *state;
}

uint64_t Thread::GetThreadId() {
  return threadId;
}

bool Thread::Retain() {
  AssertCritical();
  ScopeCriticalLock scope(&retainLock);
  if (isKilled) return false;
  if (!GetTask()->Retain()) return false;
  ++retainCount;
  
  return true;
}

void Thread::Release() {
  AssertCritical();
  bool shouldCleanup;
  Task * t = GetTask();
  {
    ScopeCriticalLock scope(&retainLock);
    assert(retainCount != 0);
    shouldCleanup = !--retainCount && isKilled;
  }
  
  if (shouldCleanup) {
    GarbageThread::GetGlobal().Push(this);
  } else {
    // we can't use GetTask() because we may have been deallocated by a
    // different Release() call in some situations
    t->Release();
  }
}

void Thread::CleanupGarbage() {
  AssertNoncritical();
  {
    ScopeCritical critical;
    Scheduler::GetGlobal().RemoveThread(this);
  }
  GetTask()->RemoveThread(this);
  {
    ScopeCritical critical;
    GetTask()->Release();
  }
  Delete();
}

}
