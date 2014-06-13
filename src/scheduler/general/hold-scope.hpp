#ifndef __SCHEDULER_HOLD_SCOPE_HPP__
#define __SCHEDULER_HOLD_SCOPE_HPP__

namespace OS {

class Task;

class HoldScope {
public:
  HoldScope(Task * t); // @ambicritical
  ~HoldScope(); // @ambicritical
  
  bool DidHold(); // @ambicritical
  
protected:
  Task * task;
  bool didHold;
};

}

#endif
