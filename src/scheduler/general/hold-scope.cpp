#include <scheduler/general/hold-scope.hpp>
#include <scheduler/general/task.hpp>
#include <arch/general/hardware-thread.hpp>
#include <critical>

namespace OS {

HoldScope::HoldScope(Task * t) : task(t) {
  ScopeCritical scope;
  didHold = t->Hold();
}

HoldScope::HoldScope() {
  ScopeCritical scope;
  
  Thread * th = HardwareThread::GetThread();
  assert(th != NULL);
  task = th->GetTask();
  didHold = task->Hold();
}

HoldScope::~HoldScope() {
  if (didHold) {
    ScopeCritical scope;
    task->Unhold();
  }
}

bool HoldScope::DidHold() {
  return didHold;
}

Task * HoldScope::GetTask() {
  return task;
}

}
