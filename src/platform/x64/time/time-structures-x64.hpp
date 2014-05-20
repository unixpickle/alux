#ifndef __PLATFORM_X64_TIME_STRUCTURES_X64_HPP
#define __PLATFORM_X64_TIME_STRUCTURES_X64_HPP

#include <cstdint>

namespace OS {
namespace x64 {

class TimeCalibration {
public:
  uint64_t lapicClock;
  uint64_t tscOffset; // if applicable
};

}
}

#endif

