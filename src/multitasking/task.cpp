#include <multitasking/task.hpp>
#include <multitasking/thread.hpp>
#include <utilities/critical.hpp>
#include <utilities/lock.hpp>
#include <cstddef>

namespace OS {

void Task::AddThread(Thread * thread) {
  AssertCritical();
  ScopeLock scope(&threadsLock);
  thread->taskNext = firstThread;
  thread->taskLast = NULL;
  firstThread = thread;
}

void Task::RemoveThread(Thread * th) {
  AssertCritical();
  ScopeLock scope(&threadsLock);
  if (firstThread == th) {
    firstThread = th->taskNext;
    if (firstThread) {
      firstThread->taskLast = NULL;
    }
  }
  th->taskNext = NULL;
  th->taskLast = NULL;
}

bool Task::Retain() {
  AssertCritical();
  ScopeCriticalLock scope(&lifeLock);
  if (!holdCount && isKilled) return false;
  retainCount++;
  return true;
}

void Task::Release() {
  AssertCritical();
  ScopeCriticalLock scope(&lifeLock);
  // TODO: release and then (if killed) push a kill thread
}

bool Task::Hold() {
  AssertCritical();
  ScopeCriticalLock scope(&lifeLock);
  if (!holdCount && isKilled) return false;
  holdCount++;
  return true;
}

void Task::Unhold() {
  AssertCritical();
  ScopeCriticalLock scope(&lifeLock);
  // TODO: unhold and then (if killed) push a kill thread
}

void Task::Kill() {
  AssertCritical();
}

}
