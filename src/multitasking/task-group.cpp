#include <multitasking/task-group.hpp>
#include <multitasking/task.hpp>
#include <utilities/critical.hpp>
#include <utilities/lock.hpp>
#include <cstddef>

namespace OS {

void TaskGroup::AddTask(Task * task) {
  AssertCritical();
  ScopeLock scope(&tasksLock);
  task->groupNext = firstTask;
  task->groupLast = NULL;
  firstTask = task;
}

void TaskGroup::RemoveTask(Task * task) {
  AssertCritical();
  ScopeLock scope(&tasksLock);
  if (firstTask == task) {
    firstTask = task->groupNext;
    if (firstTask) {
      firstTask->groupLast = NULL;
    }
  }
  task->groupNext = NULL;
  task->groupLast = NULL;
}

bool TaskGroup::Retain() {
  AssertCritical();
  ScopeCriticalLock scope(&lifeLock);
  if (!holdCount && isKilled) return false;
  retainCount++;
  return true;
}

void TaskGroup::Release() {
  AssertCritical();
  ScopeCriticalLock scope(&lifeLock);
  // TODO: release and then (if killed) push a kill thread
}

bool TaskGroup::Hold() {
  AssertCritical();
  ScopeCriticalLock scope(&lifeLock);
  if (!holdCount && isKilled) return false;
  holdCount++;
  return true;
}

void TaskGroup::Unhold() {
  AssertCritical();
  ScopeCriticalLock scope(&lifeLock);
  // TODO: unhold and then (if killed) push a kill thread
}

void TaskGroup::Kill() {
  AssertCritical();
}

void TaskGroup::Cleanup() {
  // TODO: here, unschedule each task, close sockets, etc.
  AssertCritical();
}

}
