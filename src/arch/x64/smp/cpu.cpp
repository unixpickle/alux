#include <arch/x64/smp/cpu.hpp>
#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/interrupts/vectors.hpp>
#include <arch/x64/segments/gdt.hpp>
#include <critical>

namespace OS {

HardwareThread & HardwareThread::GetCurrent() {
  return x64::CPUList::GetGlobal().GetCurrent();
}

namespace x64 {

CPU::CPU(uint32_t _apicId) : apicId(_apicId) {
  AssertNoncritical();
  
  tss = new TSS();
  uint16_t sel = GDT::GetGlobal().AddTSS(tss);
  
  // initialize this CPU entry
  dedicatedStack = (void *)((new uint8_t[0x4000]) + 0x4000);
  tss->rsp[0] = (uint64_t)dedicatedStack;
  tss->ist[0] = tss->rsp[0];
  tssSelector = sel;
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
