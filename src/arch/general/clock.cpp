#include <arch/general/clock.hpp>
#include <utilities/critical.hpp>
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
  uint64_t divide = 60000000L;
  uint64_t tpm = GetTicksPerMin();
  while (Log2Ceil(micros) + Log2Ceil(tpm) > 63) {
    divide <<= 1;
    tpm >>= 1;
  }
  Sleep((micros * tpm) / divide);
}

}
