#ifndef __X64_TICK_TIMER_HPP__
#define __X64_TICK_TIMER_HPP__

#include <arch/general/tick-timer.hpp>

namespace OS {

namespace x64 {

class TickTimer : public OS::TickTimer {
public:
  static void Initialize();
  static TickTimer & GetGlobal();
  
  virtual void SaveAndTick();
  virtual void SetTimeout(uint64_t delay, bool precision);
  virtual void ClearTimeout();
  virtual void WaitTimeout() OS_NORETURN;
};

void LapicTickMethod();

}

}

#endif
