#ifndef __SCHEDULER_CONTEXT_HPP__
#define __SCHEDULER_CONTEXT_HPP__

#include <common>
#include <cstdint>

namespace OS {

class Loop;
class Task;
class Thread;

class Context {
public:
  /**
   * @critical
   */
  static Context * GetCurrent();
  
  /**
   * @critical
   */
  Task * GetTask();
  
  /**
   * @critical
   */
  Thread * GetThread();
  
  /**
   * @param task The reference to this will be consumed.
   * @param thread A thread.
   * @critical
   */
  void SetTaskAndThread(Task * task, Thread * thread);
  
  /**
   * @ambicritical
   */
  virtual Loop & GetLoop();
  
private:
  uint64_t currentLock OS_ALIGNED(8); // @critical
  Task * currentTask; // retained
  Thread * currentThread;
};

}

#endif
