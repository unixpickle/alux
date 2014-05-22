#include "pit-x64.hpp"

namespace OS {
namespace x64 {

static PIT mainPit;

void PIT::IntHandler() {
  __asm__("lock incq (%0)" : : "r" (&ticks));
  GetLocalAPIC().SendEOI();
}

void PIT::Initialize() {
  new(&mainPit) PIT();
  SetIntRoutine(IntVectors::PIT, PitInterruptHandler);
}

PIT & PIT::GetGlobal() {
  return mainPit;
}

void PIT::Register() {
  GetBaseIOAPIC().MapIRQ(0, IntVectors::PIT);
}

void PIT::Deregister() {
  GetBaseIOAPIC().MaskIRQ(IntVectors::PIT);
}

PIT::PIT() : divide(0), ticks(0) { }

void PIT::SetDivisor(uint16_t aDiv) {
  divide = aDiv;
  *((volatile uint64_t *)&ticks) = 0;
}

uint64_t PIT::GetTime() {
  return *((volatile uint64_t *)&ticks);
}

uint64_t PIT::GetTicksPerMin() {
  return 71590920L / divide;
}

void PIT::Sleep(uint64_t time) {
  uint64_t until = GetTime() + time;
  while (GetTime() < until);
}

void PIT::WaitUntil(uint64_t deadline) {
  while (GetTime() < deadline);
}

void PitInterruptHandler() {
  PIT::GetGlobal().IntHandler();
}

}
}
