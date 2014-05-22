#include "time-x64.hpp"

namespace OS {

  static Clock * mainClock;

  namespace x64 {

    Clock ** SystemClockPointer() {
      return &mainClock;
    }

  }

  Clock & GetSystemClock() {
    return *mainClock;
  }

}
