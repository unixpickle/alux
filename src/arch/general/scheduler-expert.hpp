#include <scheduler/base/task.hpp>
#include <scheduler/base/thread.hpp>

namespace OS {

class SchedulerExpert {
public:
  static SchedulerExpert & GetGlobal();
  
  /**
   * Ask the architecture-specific code for a task that is appropriate for
   * running kernel threads.
   * @noncritical
   */
  virtual Task * CreateKernelTask() = 0;
 
  /**
   * Ask the architecture-specific code for a thread to run under a kernel
   * task.
   * @param kernTask The owning task
   * @param func The entry point of the thread. The behaviour is undefined if
   * you return from this function.
   * @noncritical
   */
  virtual Thread * CreateKernelThread(Task * kernTask, void * func) = 0;
 
  /**
   * Create a kernel thread, calling `func` and passing one pointer argument,
   * `argument`.
   * @noncritical
   */
  virtual Thread * CreateKernelThread(Task * kernTask, void * func,
                                      void * argument) = 0;
 
  /**
   * This is the fundamental method that a scheduler uses to save the
   * execution state of the current thread before switching to the next one. It
   * saves its *return state* to the thread and then calls Tick() on the
   * global Scheduler from a different context. This means that the next time
   * the saved thread is run, it will be as if the SaveAndTick() method is
   * returning.
   * @critical
   */
  virtual void SaveAndTick() = 0;
  
  /**
   * Set a timeout after which the SaveAndTick() method will automatically be
   * called (if the HardwareThread is in a critical section, then the call is
   * delayed until a noncritical section).
   * @critical
   */
  virtual void SetTimeout(uint64_t delay, bool precision) = 0; // @critical
  
  /**
   * Unset any timeout set on the current HardwareThread.
   * @critical
   */
  virtual void ClearTimeout() = 0; // @critical
  
  /**
   * Wait for a timeout to occur (i.e. SaveAndTick() to be called). If you have
   * not set any timeouts, this will hang until a Wait() call is made on the
   * waiting HardwareThread.
   * @discussion By nature, this method will *never* return.
   * @critical -> @noncritical (no return)
   */
  virtual void WaitTimeout() = 0; // @critical -> @noncritical (no return)
  
};

}
