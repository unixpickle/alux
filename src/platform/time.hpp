#ifndef __PLATFORM_TIME_HPP__
#define __PLATFORM_TIME_HPP__

#include <cstdint>
#include <utilities/logs.hpp>

namespace OS {

class Clock {
public:
  virtual uint64_t GetTime() = 0;
  virtual uint64_t GetTicksPerMin() = 0;

  virtual uint64_t WaitUntil(uint64_t deadline) {
    while (GetTime() < deadline);
  }

  virtual uint64_t Sleep(uint64_t ticks) {
    WaitUntil(ticks + GetTime());
  }

  virtual uint64_t MicroSleep(uint64_t micros) {
    Sleep((micros * GetTicksPerMin()) / 60000000L);
  }
};

Clock & GetSystemClock();
void ShortSleep(uint64_t nanoSec);

}

#endif
