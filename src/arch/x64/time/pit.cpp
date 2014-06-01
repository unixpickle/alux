#include <arch/x64/time/pit.hpp>
#include <arch/x64/interrupts/ioapic.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/interrupts/irt.hpp>
#include <arch/x64/interrupts/vectors.hpp>
#include <new>

namespace OS {

namespace x64 {

static PIT mainPit;

void PIT::IntHandler() {
  ++counter;
  LAPIC::GetCurrent().SendEOI();
}

void PIT::Initialize() {
  new(&mainPit) PIT();
  IRT::GetGlobal()[IntVectors::PIT] = PitInterruptHandler;
}

PIT & PIT::GetGlobal() {
  return mainPit;
}

void PIT::Start() {
  IOAPIC::GetBase().MapIRQ(0, IntVectors::PIT);
}

void PIT::Stop() {
  IOAPIC::GetBase().MaskIRQ(IntVectors::PIT);
}

PIT::PIT() : divide(0), counter(0) { }

void PIT::SetDivisor(uint16_t aDiv) {
  divide = aDiv;
  counter = 0;
}

uint64_t PIT::GetTime() {
  return counter.GetValue();
}

uint64_t PIT::GetTicksPerMin() {
  return 71590920L / divide;
}

void PitInterruptHandler() {
  PIT::GetGlobal().IntHandler();
}

}

}
