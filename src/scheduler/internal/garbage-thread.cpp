#include <scheduler/internal/garbage-thread.hpp>
#include <scheduler/internal/kernel-task.hpp>
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
  ScopeCriticalLock scope(&lock);
  
  t->garbageNext = firstTask;
  firstTask = t;
  
  Wakeup();
}

void GarbageThread::PushThread(Thread * t) {
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  
  t->garbageNext = firstThread;
  firstThread = t;
  
  Wakeup();
}

void GarbageThread::Initialize() {
  AssertNoncritical();
  Task * task = KernelTask::GetGlobal().GetTask();
  thread = Thread::NewKernel(task, (void *)&GarbageThread::CallMain);
  
  task->AddThread(thread);
  
  ScopeCritical critical;
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
  SetCritical(true);
  while (1) {
    LockHold(&lock);
    Task * task = PopTask();
    Thread * thread = PopThread();
    if (!task && !thread) {
      Scheduler::GetGlobal().SetInfiniteTimeout(&lock);
      continue;
    }
    LockRelease(&lock);
    
    SetCritical(false);
    if (task) task->Delete();
    if (thread) {
      thread->GetTask()->RemoveThread(thread);
      thread->GetTask()->Release();
      thread->Delete();
    }
    SetCritical(true);
  }
}

Task * GarbageThread::PopTask() {
  if (!firstTask) return NULL;
  Task * result = firstTask;
  firstTask = result->garbageNext;
  result->garbageNext = NULL;
  return result;
}

Thread * GarbageThread::PopThread() {
  if (!firstThread) return NULL;
  Thread * result = firstThread;
  firstThread = result->garbageNext;
  result->garbageNext = NULL;
  return result;
}

}
