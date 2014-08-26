#include "time.hpp"
#include <anarch/api/clock-module>
#include <anarch/api/clock>

namespace Alux {

anarch::SyscallRet GetNanoTimeSyscall() {
  anarch::Clock & clock = anarch::ClockModule::GetGlobal().GetClock();
  uint64_t ticks = clock.GetTicks();
  uint64_t ns = clock.GetMicrosPerTick().ScaleInteger(ticks) * 1000;
  return anarch::SyscallRet::Integer64(ns);
}

}
