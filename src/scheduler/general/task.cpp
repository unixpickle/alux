#include <scheduler/general/task.hpp>
#include <scheduler/general/garbage-thread.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <utilities/lock.hpp>
#include <utilities/critical.hpp>

namespace OS {

Task * Task::New(bool forKernel) {
  // TODO: use a slab here
  return new Task(forKernel);
}

Task::Task(bool forKernel) : ArchTask(forKernel) {
  AssertNoncritical();
  // TODO: here, allocate a PID
}

Task::~Task() {
  AssertNoncritical();
  // no need to lock our threads list anymore since it can't change externally
  // while nobody can retain us.
  while (firstThread) {
    SetCritical(true);
    Scheduler::GetGlobal().RemoveThread(firstThread);
    SetCritical(false);
    Thread * th = firstThread;
    firstThread = th->taskNext;
    th->Delete();
  }
  
  // TODO: get rid of the PID here, close sockets, etc.
}

void Task::Delete() {
  delete this; // TODO: use a slab here
}

void Task::AddThread(Thread * th) {
  AssertCritical();
  ScopeCriticalLock lock(&threadsLock);
  th->taskNext = firstThread;
  th->taskLast = NULL;
  if (firstThread) {
    firstThread->taskLast = th;
  }
  firstThread = th;
}

void Task::RemoveThread(Thread * th) {
  AssertCritical();
  ScopeCriticalLock lock(&threadsLock);
  if (th == firstThread) {
    firstThread = th->taskNext;
    if (firstThread) firstThread->taskLast = NULL;
  } else {
    th->taskLast->taskNext = th->taskNext;
    if (th->taskNext) {
      th->taskNext = th->taskLast;
    }
  }
}

bool Task::Retain() {
  AssertCritical();
  ScopeCriticalLock lock(&stateLock);
  if (isKilled && !holdCount) return false;
  retainCount++;
  return true;
}

void Task::Release() {
  AssertCritical();
  ScopeCriticalLock lock(&stateLock);
  if (!--retainCount && !holdCount && isKilled) {
    Terminate();
  }
}

bool Task::Hold() {
  AssertCritical();
  ScopeCriticalLock lock(&stateLock);
  if (isKilled && !holdCount) return false;
  holdCount++;
  return true;
}

void Task::Unhold() {
  AssertCritical();
  ScopeCriticalLock lock(&stateLock);
  if (!--holdCount && !retainCount && isKilled) {
    Terminate();
  }
}

void Task::Kill(uint64_t status) {
  AssertCritical();
  ScopeCriticalLock lock(&stateLock);
  assert(retainCount || holdCount);
  if (!isKilled) {
    isKilled = true;
    killStatus = status;
  }
}

// PRIVATE

void Task::Terminate() {
  GarbageThread::GetGlobal().PushTask(this);
}

}
