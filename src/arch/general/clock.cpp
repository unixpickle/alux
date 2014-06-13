#include <arch/general/clock.hpp>
#include <utilities/frac.hpp>
#include <critical>
#include <math>

namespace OS {

void Clock::WaitUntil(uint64_t deadline) {
  AssertNoncritical();
  while (GetTime() < deadline);
}

void Clock::Sleep(uint64_t ticks) {
  AssertNoncritical();
  WaitUntil(ticks + GetTime());
}

void Clock::MicroSleep(uint64_t micros) {
  AssertNoncritical();
  Frac64 ratio(GetTicksPerMin(), 60000000L);
  Sleep(ratio.Multiply(micros));
}

}
