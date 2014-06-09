#include <arch/x64/smp/cpu.hpp>
#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/interrupts/vectors.hpp>
#include <utilities/critical.hpp>

namespace OS {

CPU & CPU::GetCurrent() {
  return x64::CPUList::GetGlobal().GetCurrent();
}

namespace x64 {

CPU::CPU(uint32_t _apicId) : apicId(_apicId) {
}
  
uint32_t CPU::GetAPICID() {
  return apicId;
}

int CPU::GetIndex() {
  return (size_t)GetAPICID();
}

void CPU::Wake() {
  AssertCritical();
  LAPIC & lapic = LAPIC::GetCurrent();
  lapic.SendIPI(GetAPICID(), IntVectors::LapicTimer);
}

}

}
