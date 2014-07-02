#include <scheduler/general/hold-scope.hpp>
#include <scheduler/general/task.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <arch/general/hardware-thread.hpp>
#include <critical>
#include <panic>

namespace OS {

HoldScope::HoldScope() : wasCritical(GetCritical()) {
  SetCritical(true);
  
  thread = HardwareThread::GetThread();
  assert(thread != NULL);
  task = thread->GetTask();
  assert(task != NULL);
  
  // support nesting scopes that still allow for Exit() calls.
  if (!thread->isHoldingTask) {
    if (!task->Hold()) {
      Thread::Exit();
    }
    thread->isHoldingTask = true;
    didHold = true;
  }
  
  SetCritical(false);
}

HoldScope::~HoldScope() {
  if (didHold) {
    thread->isHoldingTask = false;
    task->Unhold();
  }
  SetCritical(wasCritical);
}

Task * HoldScope::GetTask() {
  return task;
}

Thread * HoldScope::GetThread() {
  return thread;
}

void HoldScope::Exit(uint64_t status) {
  SetCritical(true);
  task->Unhold();
  Task::Exit(status);
}

}
