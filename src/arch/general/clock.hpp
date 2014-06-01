#ifndef __GENERAL_CLOCK_HPP__
#define __GENERAL_CLOCK_HPP__

#include <cstdint>

namespace OS {

class Clock {
public:
  static Clock & GetGlobal();
  
  /**
   * @noncritical
   */
  virtual uint64_t GetTime() = 0;
  
  /**
   * @noncritical
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

}

#endif
