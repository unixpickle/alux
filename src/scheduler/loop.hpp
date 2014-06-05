#ifndef __SCHEDULER_LOOP_HPP__
#define __SCHEDULER_LOOP_HPP__

#include <common>

namespace OS {

class Thread;
class Task;

class Loop {
public:
  virtual void PushThread(Thread * thread); // @critical
  virtual Thread * PopThread(Task ** task); // @critical
  virtual bool RemoveThread(Thread * thread); // @critical
  
protected:
  uint64_t lock OS_ALIGNED(8); // @critical
};

}

#endif
