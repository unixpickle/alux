#ifndef __SCHEDULER_THREAD_HPP__
#define __SCHEDULER_THREAD_HPP__

#include <scheduler-specific/thread-info.hpp>

namespace OS {

class Task;

class Thread {
public:
  ThreadInfo userInfo;
  
  Thread(Task * owner); // @noncritical
  virtual ~Thread(); // @noncritical
  virtual void Delete() = 0; // @noncritical
  Task * GetTask(); // @ambicritical
  
  virtual void Run() = 0; // @critical -> @noncritical (no return)
  
protected:
  Thread * taskNext, * taskLast;
  friend class Task;
  
private:
  Task * task;
};

}

#endif