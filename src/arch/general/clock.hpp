#ifndef __GENERAL_CLOCK_HPP__
#define __GENERAL_CLOCK_HPP__

#include <cstdint>
#include <module/module.hpp>

namespace OS {

class Clock {
public:
  static Clock & GetClock(); // uses the ClockModule global
  
  /**
   * @ambicritical
   */
  virtual uint64_t GetTime() = 0;
  
  /**
   * @ambicritical
   */
  virtual uint64_t GetTicksPerMin() = 0;

  /**
   * @noncritical
   */
  virtual void WaitUntil(uint64_t deadline);
  
  /**
   * @noncritical
   */
  virtual void Sleep(uint64_t ticks);
  
  /**
   * @noncritical
   */
  virtual void MicroSleep(uint64_t micros);
};

class ClockModule : public Module {
public:
  static ClockModule & GetGlobal();
  
  virtual Clock & GetClock() = 0;
};

}

#endif
