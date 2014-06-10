#include <scheduler/general/garbage-thread.hpp>
#include <scheduler/general/task.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <arch/general/failure.hpp>
#include <utilities/critical.hpp>
#include <utilities/lock.hpp>
#include <new>

namespace OS {

static GarbageThread globalGarbage;

void GarbageThread::Initialize(Task * kernelTask) {
  new(&globalGarbage) GarbageThread(kernelTask);
}

GarbageThread & GarbageThread::GetGlobal() {
  return globalGarbage;
}

GarbageThread::GarbageThread() : Thread(NULL, false) {
  Panic("GarbageThread::GarbageThread() - present only for the compiler");
}

GarbageThread::GarbageThread(Task * owner) : Thread(owner, true) {
  SetKernelCall((void *)&GarbageThread::CallMain);
}

void GarbageThread::Delete() {
  Panic("GarbageThread::Delete() - global object cannot be deleted");
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

void GarbageThread::Wakeup() {
  AssertCritical();
  Scheduler::GetGlobal().ClearTimeout(&GarbageThread::GetGlobal());
}

void GarbageThread::CallMain() {
  GarbageThread::GetGlobal().Main();
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
