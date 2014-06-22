#ifndef __SCHEDULER_HOLD_SCOPE_HPP__
#define __SCHEDULER_HOLD_SCOPE_HPP__

#include <cstdint>
#include <macros>

namespace OS {

class Task;

class HoldScope {
public:
  HoldScope(); // @ambicritical (x) -> @noncritical
  ~HoldScope(); // @ambicritical -> @ambicritical (x)
  
  bool DidHold(); // @ambicritical
  Task * GetTask();
  void Exit(uint64_t status) OS_NORETURN;
  
protected:
  Task * task;
  bool didHold;
  bool wasCritical;
};

}

#endif
