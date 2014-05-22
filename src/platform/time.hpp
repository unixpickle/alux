#ifndef __PLATFORM_TIME_HPP__
#define __PLATFORM_TIME_HPP__

#include <cstdint>

namespace OS {

class Clock {
public:
  virtual uint64_t GetTime() = 0;
  virtual uint64_t GetTicksPerMin() = 0;
};

Clock & GetSystemClock();

}

#endif
