#include "hold-scope.hpp"
#include "../scheduler/scheduler.hpp"
#include <anarch/critical>

namespace Alux {

HoldScope::HoldScope() {
  thread = Thread::GetCurrent();
  
  assert(thread != NULL);
  task = &thread->GetTask();
  didHold = !thread->holdingTask;
  if (didHold) {
    if (!task->Hold()) {
      thread->Kill();
      task->GetScheduler().Yield();
      __builtin_unreachable();
    }
    thread->holdingTask = true;
  }
  
  // we can only leave the critical section once we know the task is held
  wasCritical = anarch::GetCritical();
  if (wasCritical) anarch::SetCritical(false);
}

HoldScope::~HoldScope() {
  // leave the critical section first so that we can safely unhold the task
  anarch::SetCritical(wasCritical);
  if (didHold) {
    thread->holdingTask = false;
    task->Unhold();
  }
}

void HoldScope::ExitThread() {
  // because we enter a critical section, the task cannot be deallocated or
  // fully released until we Yield()
  anarch::SetCritical(true);
  thread->Kill();
  task->Unhold();
  task->GetScheduler().Yield();
  __builtin_unreachable();
}

void HoldScope::ExitTask(int status) {
  // because we enter a critical section, the task cannot be deallocated or
  // fully released until we Yield()
  anarch::SetCritical(true);
  thread->Kill();
  task->Kill(status);
  task->Unhold();
  task->GetScheduler().Yield();
  __builtin_unreachable();
}

}
