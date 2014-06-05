#ifndef __SCHEDULER_THREAD_HPP__
#define __SCHEDULER_THREAD_HPP__

#include <cstdint>
#include <common>

namespace OS {

class Task;
class Loop;

class Thread {
public:
  /**
   * Create a new thread that is owned by a given task.
   * @param container A referenced task.
   * @noncritical
   */
  Thread(Task * container);
  
  /**
   * @noncritical
   */
  virtual ~Thread();
  
  /**
   * Returns a retained reference to the parent task or NULL if the task has
   * been killed.
   * @critical
   */
  virtual Task * GetTask();
  
protected:
  Task * task;
  
  uint64_t loopLock OS_ALIGNED(8); // @critical
  Loop * loop;
  Thread * loopNext, * loopLast;
  
  friend class Loop;
};

}

#endif
