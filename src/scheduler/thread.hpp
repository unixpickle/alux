#ifndef __SCHEDULER_THREAD_HPP__
#define __SCHEDULER_THREAD_HPP__

#include <cstdint>
#include <common>

namespace OS {

class Task;

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
  ~Thread();
  
  class FxState {
    // TODO: information from SSE2 regs
  } OS_PACKED;
  
  class State {
    FxState * fxState; // NULL = floating-point uninitialized
    uint64_t rsp;
    uint64_t rbp;
    uint64_t rip;
    uint64_t eflags;
  } OS_PACKED;
  
  Thread * next, * last;
  Thread * queueNext, * queueLast;
  
  /**
   * Returns a retained reference to the parent task or NULL if the task has
   * been killed.
   * @critical
   */
  virtual Task * GetTask();
  
  /**
   * Returns a read-write reference to the state
   * @ambicritical
   */
  virtual State & GetState();
  
protected:
  Task * task;
  
  uint64_t stateLock OS_ALIGNED(8);
  State state;
  
  uint64_t pollLock OS_ALIGNED(8);
  bool isPolling;
  uint64_t pollDescriptor;
};

}

#endif
