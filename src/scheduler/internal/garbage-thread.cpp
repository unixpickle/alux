#include <scheduler/internal/garbage-thread.hpp>
#include <scheduler/general/task.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <critical>
#include <lock>
#include <panic>
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

void GarbageThread::PushTask(Task * t) {
  AssertCritical();
  {
    ScopeCriticalLock scope(&tasksLock);
    t->garbageNext = firstTask;
    firstTask = t;
  }
  Wakeup();
}

void GarbageThread::Initialize() {
  AssertNoncritical();
  Task * task = KernelTask::GetGlobal().GetTask();
  thread = Thread::NewKernel(task, &GarbageThread::CallMain);
  
  ScopeCritical critical;
  task->AddThread(thread);
  Scheduler::GetGlobal().AddThread(thread);
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
  while (1) {
    SetCritical(true);
    Task * t = PopTask();
    SetCritical(false);
    if (!t) {
      SetCritical(true);
      Scheduler::GetGlobal().SetInfiniteTimeout();
      SetCritical(false);
      continue;
    }
    t->Delete();
  }
}

Task * GarbageThread::PopTask() {
  AssertCritical();
  ScopeCriticalLock scope(&tasksLock);
  
  if (!firstTask) return NULL;
  Task * result = firstTask;
  firstTask = result->garbageNext;
  result->garbageNext = NULL;
  return result;
}

}
