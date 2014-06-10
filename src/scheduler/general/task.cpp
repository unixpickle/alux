#include <scheduler/general/task.hpp>
#include <scheduler/general/thread.hpp>
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
  // TODO: here, destroy all the threads, deregister the PID, close sockets,
  // etc.
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
  // TODO: here, push the task to the garbage thread
}

}
