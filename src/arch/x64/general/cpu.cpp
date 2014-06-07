#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/interrupts/vectors.hpp>
#include <arch/general/clock.hpp>
#include <utilities/critical.hpp>
#include <utilities/frac.hpp>

namespace OS {

void CPU::HaltCurrent() {
  AssertCritical();
  x64::LAPIC::GetCurrent().ClearTimeout();
  WaitUntilTick();
}

CPU & CPU::GetCurrent() {
  AssertCritical();
  return x64::CPUList::GetGlobal().GetCurrent();
}

void CPU::WaitUntilTick() {
  AssertCritical();
  __asm__("sti");
  while (1) {
    __asm__ __volatile__("hlt");
  }
}

void CPU::SetTimeout(uint64_t clockTicks) {
  AssertCritical();
  x64::CPU & cpu = x64::CPUList::GetGlobal().GetCurrent();

  uint64_t regularClock = Clock::GetGlobal().GetTicksPerMin();
  Frac64 conversion(cpu.frequencies.lapic, regularClock);
  uint64_t lapicTicks = conversion.Multiply(clockTicks);

  x64::LAPIC::GetCurrent().SetTimeout(x64::IntVectors::LapicTimer, lapicTicks);
}

size_t CPU::GetCount() {
  return x64::CPUList::GetGlobal().GetCount();
}

CPU & CPU::GetAt(size_t index) {
  return x64::CPUList::GetGlobal()[index];
}

}