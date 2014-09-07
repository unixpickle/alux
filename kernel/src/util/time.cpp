#include "time.hpp"
#include <anarch/api/clock-module>
#include <anarch/api/clock>

namespace Alux {

uint64_t NanosFromNow(uint64_t nanos) {
  anarch::Clock & clock = anarch::ClockModule::GetGlobal().GetClock();
  uint64_t ticks = clock.GetTicks();
  return ticks + clock.GetMicrosPerTick().Flip().ScaleInteger(nanos / 1000);
}

}
