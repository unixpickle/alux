#include <arch/x64/time/clock.hpp>

namespace OS {

static Clock * sysClock;

Clock & Clock::GetGlobal() {
  return *sysClock;
}

void SetCurrentClock(Clock * clock) {
  sysClock = clock;
}

}
