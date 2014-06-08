#ifndef __SCHEDULER_THREAD_HPP__
#define __SCHEDULER_THREAD_HPP__

#include <scheduler_specific/thread-info.hpp>
#include <utilities/linked-list.hpp>

namespace OS {

class Task;

class Thread {
public:
  ThreadInfo userInfo;
  LinkedListLink<Thread> taskLink;
  
  Thread(Task * owner); // @noncritical
  virtual ~Thread(); // @noncritical
  virtual void Delete() = 0; // @noncritical
  Task * GetTask(); // @ambicritical
  
  virtual void Run() = 0; // @critical -> @noncritical (no return)
  
private:
  Task * task;
};

}

#endif