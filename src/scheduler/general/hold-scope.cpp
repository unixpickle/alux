#include <scheduler/general/hold-scope.hpp>
#include <scheduler/general/task.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <arch/general/hardware-thread.hpp>
#include <critical>
#include <panic>

namespace OS {

HoldScope::HoldScope() : wasCritical(GetCritical()) {
  SetCritical(true);
  
  Thread * th = HardwareThread::GetThread();
  assert(th != NULL);
  task = th->GetTask();
  didHold = task->Hold();
  
  SetCritical(false);
}

HoldScope::~HoldScope() {
  if (didHold) {
    SetCritical(true);
    task->Unhold();
  }
  SetCritical(wasCritical);
}

bool HoldScope::DidHold() {
  return didHold;
}

Task * HoldScope::GetTask() {
  assert(didHold);
  return task;
}

void HoldScope::Exit(uint64_t status) {
  assert(didHold);
  SetCritical(true);
  task->Unhold();
  Scheduler::GetGlobal().ExitTask(status);
}

}
