#include <scheduler/base/task.hpp>
#include <utilities/lock.hpp>
#include <utilities/critical.hpp>

namespace OS {

Task::~Task() {
  // TODO: here, destroy all the threads, deregister the PID, close sockets,
  // etc.
}

void Task::AddThread(Thread * th) {
  AssertCritical();
  ScopeCriticalLock lock(&threadsLock);
  threads.PushBack(th);
}

void Task::RemoveThread(Thread * th) {
  AssertCritical();
  ScopeCriticalLock lock(&threadsLock);
  threads.Remove(th);
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
