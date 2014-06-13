#include <scheduler/general/hold-scope.hpp>
#include <scheduler/general/task.hpp>
#include <critical>

namespace OS {

HoldScope::HoldScope(Task * t) : task(t) {
  ScopeCritical scope;
  didHold = t->Hold();
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

}
