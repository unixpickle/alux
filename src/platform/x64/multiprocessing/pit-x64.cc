#include "pit-x64.hpp"

namespace OS {

namespace x64 {

static uint64_t tickCount = 0;
static volatile uint64_t * tickPtr = &tickCount;

void PitSetDivisor(uint16_t div) {
  *tickPtr = 0;
  // TODO: figure out speed inconsistency
  OutB(0x43, 0x34); // channel 0, lobyte/hibyte, rate generator
  OutB(0x40, (uint8_t)(div & 0xff));
  OutB(0x40, (uint8_t)((div >> 8) & 0xff));
}

void PitInterruptHandler() {
  assert(!GetInterruptsEnabled());
  __asm__("lock incq (%0)" : : "r" (tickPtr));
  GetLocalAPIC().SendEOI();
}

void PitSleep(uint64_t ticks) {
  uint64_t until = *tickPtr + ticks;
  while (*tickPtr < until);
}

}

}
