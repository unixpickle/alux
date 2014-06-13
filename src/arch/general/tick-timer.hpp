#ifndef __GENERAL_TICK_TIMER_HPP__
#define __GENERAL_TICK_TIMER_HPP__

#include <scheduler/general/task.hpp>
#include <scheduler/general/thread.hpp>
#include <macros>

namespace OS {

class TickTimer {
public:
  static TickTimer & GetCurrent(); // @critical
 
  /**
   * This is the fundamental method that a scheduler uses to save the
   * execution state of the current thread before switching to the next one. It
   * saves its *return state* to the thread and then calls Tick() on the
   * global Scheduler from a different context. This means that the next time
   * the saved thread is run, it will be as if the SaveAndTick() method is
   * returning.
   * @critical -> (@noncritical) -> @critical
   */
  virtual void SaveAndTick() = 0;
  
  /**
   * Set a timeout after which the SaveAndTick() method will automatically be
   * called (if the HardwareThread is in a critical section, then the call is
   * delayed until a noncritical section).
   * @critical
   */
  virtual void SetTimeout(uint64_t delay, bool precision) = 0;
  
  /**
   * Unset any timeout set on the current HardwareThread.
   * @critical
   */
  virtual void ClearTimeout() = 0;
  
  /**
   * Wait for a timeout to occur (i.e. SaveAndTick() to be called). If you have
   * not set any timeouts, this will hang until a Wait() call is made on the
   * waiting HardwareThread.
   * @discussion By nature, this method will *never* return.
   * @critical -> @noncritical (no return)
   */
  virtual void WaitTimeout() OS_NORETURN = 0;
  
};

}

#endif
