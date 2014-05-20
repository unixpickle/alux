#ifndef __PLATFORM_X64_TIME_STRUCTURES_X64_HPP
#define __PLATFORM_X64_TIME_STRUCTURES_X64_HPP

#include <cstdint>

namespace OS {
namespace x64 {

class TimeCalibration {
public:
  uint64_t lapicClock;
  bool lapicCalibrated;

  uint64_t tscOffset;
  bool tscCalibrated;
};

class Time128 {
protected:
  uint64_t lower, higher;

public:
  uint64_t GetLower();
  uint64_t GetHigher();

  Time128(uint64_t low, uint64_t high);
  Time128(const Time128 & time);
  Time128 operator+(const Time128 & time);
  Time128 operator+(uint64_t num);
  Time128 & operator=(const Time128 & time);
};

}
}

#endif

