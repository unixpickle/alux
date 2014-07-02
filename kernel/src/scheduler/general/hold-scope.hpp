#ifndef __SCHEDULER_HOLD_SCOPE_HPP__
#define __SCHEDULER_HOLD_SCOPE_HPP__

#include <cstdint>
#include <macros>

namespace OS {

class Task;
class Thread;

class HoldScope {
public:
  HoldScope(); // @ambicritical (x) -> @noncritical
  ~HoldScope(); // @ambicritical -> @ambicritical (x)
  
  Task * GetTask();
  Thread * GetThread();
  
  void Exit(uint64_t status) OS_NORETURN;
  
protected:
  Thread * thread;
  Task * task;
  bool wasCritical;
  bool didHold = false;
};

}

#endif
