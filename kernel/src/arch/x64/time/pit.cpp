#include <arch/x64/time/pit.hpp>
#include <arch/x64/interrupts/ioapic.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/interrupts/irt.hpp>
#include <arch/x64/interrupts/vectors.hpp>
#include <arch/x64/common.hpp>
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

PIT::PIT() : counter(0) {
  
}

void PIT::Start() {
  IOAPIC::GetBase().MapIRQ(0, IntVectors::PIT);
}

void PIT::Stop() {
  IOAPIC::GetBase().MaskIRQ(IntVectors::PIT);
}

void PIT::SetDivisor(uint16_t aDiv) {
  divide = aDiv;
  counter = 0;
  
  OutB(0x43, 0x34); // channel 0, lobyte/hibyte, rate generator
  OutB(0x40, (uint8_t)(aDiv & 0xff));
  OutB(0x40, (uint8_t)((aDiv >> 8) & 0xff));
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
