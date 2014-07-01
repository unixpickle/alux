#include <scheduler/general/task.hpp>
#include <scheduler/general/thread.hpp>
#include <scheduler/internal/garbage-thread.hpp>
#include <scheduler/internal/pid-pool.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <arch/general/user-map.hpp>
#include <arch/general/global-map.hpp>
#include <arch/general/hardware-thread.hpp>
#include <critical>
#include <lock>

namespace OS {

static Atomic<uint64_t> taskCount(0);

Task * Task::New(bool kern) {
  return new Task(kern);
}

void Task::InitializeCounter() {
  new(&taskCount) Atomic<uint64_t>(0);
}

uint64_t Task::GetCounter() {
  return taskCount;
}

void Task::Exit(uint64_t status) {
  AssertCritical();
  
  Thread * th = HardwareThread::GetThread();
  assert(th != NULL);
  Task * task = th->GetTask();
  task->Kill(status);
  
  Thread::Exit();
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
  
  PIDPool::GetGlobal().FreePID(GetPID(), this);
  if (userSpace) userSpace->Delete();
  
  --taskCount;
}

void Task::Delete() {
  delete this;
}

bool Task::IsKernel() {
  return isKernel;
}

void Task::AddThread(Thread * th) {
  AssertNoncritical();
  th->threadId = threadIds.Pop();
  
  ScopeLock lock(&threadsLock);
  
  th->taskNext = firstThread;
  th->taskLast = NULL;
  if (firstThread) {
    firstThread->taskLast = th;
  }
  firstThread = th;
}

void Task::RemoveThread(Thread * th) {
  AssertNoncritical();
  
  threadIds.Push(th->threadId);
  
  ScopeLock lock(&threadsLock);
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

void Task::UnsleepThreadById(uint64_t ident) {
  AssertNoncritical();
  ScopeLock lock(&threadsLock);
  Thread * th = firstThread;
  while (th) {
    if (th->GetThreadId() == ident) break;
    th = th->taskNext;
  }
  if (!th) return;
  
  ScopeLock timeoutLock(&th->timeoutLock);
  if (th->isSleeping) {
    ScopeCritical critical;
    Scheduler::GetGlobal().ClearTimeout(th);
  } else {
    th->shouldClearTimeout = true;
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
  bool shouldTerm;
  {
    ScopeCriticalLock lock(&stateLock);
    shouldTerm = !--retainCount && !holdCount && isKilled;
  }
  if (shouldTerm) Terminate();
}

bool Task::Hold() {
  AssertCritical();
  ScopeCriticalLock lock(&stateLock);
  if (isKilled) return false;
  holdCount++;
  return true;
}

void Task::Unhold() {
  AssertCritical();
  bool shouldTerm;
  {
    ScopeCriticalLock lock(&stateLock);
    shouldTerm = !--holdCount && !retainCount && isKilled;
  }
  if (shouldTerm) Terminate();
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

uint64_t Task::GetPID() {
  return pid;
}

AddressSpace & Task::GetAddressSpace() {
  if (userSpace) return *userSpace;
  return GlobalMap::GetGlobal();
}

UserMap * Task::GetUserAddressSpace() {
  return userSpace;
}

// PROTECTED

void Task::CleanupGarbage() {
  Delete();
}

Task::Task(bool forKernel) : isKernel(forKernel) {
  AssertNoncritical();
  ++taskCount;
  
  pid = PIDPool::GetGlobal().AllocPID(this);
  
  if (isKernel) {
    userSpace = NULL;
  } else {
    userSpace = UserMap::New();
  }
}

// PRIVATE

void Task::Terminate() {
  GarbageThread::GetGlobal().Push(this);
}

}
