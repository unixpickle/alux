#ifndef __ALUX_SLEEP_STATE_HPP__
#define __ALUX_SLEEP_STATE_HPP__

#include <anarch/lock>

namespace Alux {

class Thread;

/**
 * Used for atomically sleeping and unsleeping user-space tasks.
 */
class SleepState {
public:
  SleepState(Thread &);
  
  void Cancel(); // @ambicritical
  
  /**
   * Sleep the current thread for a period of time.
   *
   * Why is this critical, you might ask? Because I don't want to encourage
   * you to hold the current task and then sleep. The problem with such a
   * practice is that, while the thread sleeps, the task could not be killed.
   *
   * @critical
   */
  static void Sleep(uint64_t nanoseconds);
  
  /**
   * Sleep the current thread until it is woken.
   *
   * See note for [Sleep] for more details on this method's criticality.
   *
   * @critical
   */
  static void SleepInfinite();
  
private:
  Thread & thread;
  
  anarch::CriticalLock lock;
  bool sleeping = false;
  bool cancelled = false;
};

}

#endif
