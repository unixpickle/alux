#ifndef __SCHEDULER_HOLD_SCOPE_HPP__
#define __SCHEDULER_HOLD_SCOPE_HPP__

namespace OS {

class Task;

class HoldScope {
public:
  HoldScope(Task * t); // @ambicritical
  HoldScope(); // @ambicritical, holds current task
  ~HoldScope(); // @ambicritical
  
  bool DidHold(); // @ambicritical
  Task * GetTask();
  
protected:
  Task * task;
  bool didHold;
};

}

#endif
