#include <scheduler/internal/garbage-thread.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <scheduler/internal/kernel-task.hpp>
#include <scheduler/general/thread.hpp>
#include <scheduler/general/task.hpp>
#include <critical>
#include <panic>
#include <lock>
#include <new>

namespace OS {

static GarbageThread gGarbage;

void GarbageThread::InitGlobal() {
  new(&gGarbage) GarbageThread();
}

GarbageThread & GarbageThread::GetGlobal() {
  return gGarbage;
}

Thread * GarbageThread::GetThread() {
  return thread;
}

void GarbageThread::Push(GarbageThread::Garbage * g) {
  ScopeCritical critical;
  ScopeCriticalLock scope(&lock);
  
  g->garbageNext = first;
  first = g;
  
  Wakeup();
}

void GarbageThread::Initialize() {
  AssertNoncritical();
  Task * task = KernelTask::GetGlobal().GetTask();
  thread = Thread::NewKernel(task, (void *)&GarbageThread::CallMain);
  
  assert(thread != NULL);
  
  task->AddThread(thread);
  
  ScopeCritical critical;
  Scheduler::GetGlobal().AddThread(thread);
  thread->Release();
}

DepList GarbageThread::GetDependencies() {
  return DepList(&KernelTask::GetGlobal(), &Scheduler::GetGlobal());
}

void GarbageThread::CallMain() {
  AssertNoncritical();
  GarbageThread::GetGlobal().Main();
}

void GarbageThread::Wakeup() {
  AssertCritical();
  Scheduler::GetGlobal().ClearTimeout(GetThread());
}

void GarbageThread::Main() {
  AssertNoncritical();
  SetCritical(true);
  while (1) {
    LockHold(&lock);
    Garbage * g = Pop();
    if (!g) {
      Scheduler::GetGlobal().SetInfiniteTimeout(&lock);
      continue;
    }
    LockRelease(&lock);
    
    SetCritical(false);
    g->CleanupGarbage();
    SetCritical(true);
  }
}

GarbageThread::Garbage * GarbageThread::Pop() {
  if (!first) return NULL;
  Garbage * res = first;
  first = first->garbageNext;
  res->garbageNext = NULL;
  return res;
}

}
