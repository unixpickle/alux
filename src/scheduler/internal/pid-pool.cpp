#include <scheduler/internal/pid-pool.hpp>
#include <scheduler/general/task.hpp>
#include <utilities/critical.hpp>
#include <utilities/lock.hpp>

namespace OS {

static PIDPool thePool;

void PIDPool::Initialize() {
  AssertNoncritical();
  new(&thePool) PIDPool();
}

PIDPool & PIDPool::GetGlobal() {
  return thePool;
}

uint64_t PIDPool::AllocPID(Task * task) {
  AssertNoncritical();
  uint64_t pid = indexSet.Pop();
  
  ScopeLock scope(&mapLock);
  uint64_t bucket = pid % IdealMaximum;
  task->pidPoolNext = buckets[bucket];
  task->pidPoolLast = NULL;
  buckets[bucket] = task;
  
  return pid;
}

void PIDPool::FreePID(uint64_t pid, Task * task) {
  AssertNoncritical();
  {
    ScopeLock scope(&mapLock);
    uint64_t bucket = pid % IdealMaximum;
    if (task->pidPoolLast) {
      task->pidPoolLast->pidPoolNext = task->pidPoolNext;
    } else {
      buckets[bucket] = task->pidPoolNext;
    }
    if (task->pidPoolNext) {
      task->pidPoolNext->pidPoolLast = task->pidPoolLast;
    }
  }
  indexSet.Push(pid);
}

Task * PIDPool::Find(uint64_t pid) {
  AssertNoncritical();
  ScopeLock scope(&mapLock);
  uint64_t bucket = pid % IdealMaximum;
  Task * t = buckets[bucket];
  while (t) {
    if (t->GetPID() == pid) {
      if (t->Retain()) {
        return t;
      } else {
        return NULL;
      }
    }
    t = t->pidPoolNext;
  }
  return NULL;
}

}
