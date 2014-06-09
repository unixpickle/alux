#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/interrupts/vectors.hpp>
#include <arch/general/failure.hpp>
#include <arch/general/clock.hpp>
#include <utilities/frac.hpp>
#include <utilities/critical.hpp>

namespace OS {

CPU & CPU::GetCurrent() {
  AssertCritical();
  return x64::CPUList::GetGlobal().GetCurrent();
}

size_t CPU::GetCount() {
  return (size_t)x64::CPUList::GetGlobal().GetCount();
}

CPU & CPU::GetAt(size_t idx) {
  return static_cast<CPU &>(x64::CPUList::GetGlobal()[(int)idx]);
}

void CPU::SaveAndTick() {
  // TODO: save the registers by pushing them to the stack, then switch stack
  // and call the destination method
  Panic("CPU::SaveAndTick - NYI");
}

void CPU::SetTimeout(uint64_t deadline, bool) {
  AssertCritical();
  x64::CPU & cpu = x64::CPUList::GetGlobal().GetCurrent();

  uint64_t regularClock = Clock::GetGlobal().GetTicksPerMin();
  Frac64 conversion(cpu.frequencies.lapic, regularClock);
  uint64_t lapicTicks = conversion.Multiply(deadline);

  x64::LAPIC::GetCurrent().SetTimeout(x64::IntVectors::LapicTimer, lapicTicks);
}

void CPU::ClearTimeout() {
  AssertCritical();
  x64::LAPIC::GetCurrent().ClearTimeout();
}

void CPU::Sleep() {
  AssertCritical();
  __asm__("sti");
  while (1) {
    __asm__ __volatile__("hlt");
  }
}

}
