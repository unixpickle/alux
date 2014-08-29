#ifndef __ALUX_SLEEP_STATE_HPP__
#define __ALUX_SLEEP_STATE_HPP__

#include <anarch/lock>

namespace Alux {

/**
 * Used for atomically sleeping and unsleeping user-space tasks.
 */
class SleepState {
public:
  void Cancel(); // @ambicritical
  
  static void Sleep(uint64_t nanoseconds); // @ambicritical
  static void SleepInfinite(); // @ambicritical
  
private:
  anarch::CriticalLock lock;
  bool sleeping = false;
  bool cancelled = false;
};

}

#endif
