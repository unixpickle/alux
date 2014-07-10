#include <arch/x64/smp/cpu.hpp>
#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/interrupts/vectors.hpp>
#include <arch/x64/segments/gdt.hpp>
#include <arch/x64/segments/gs-register.hpp>
#include <critical>

namespace OS {

HardwareThread & HardwareThread::GetCurrent() {
  return x64::CPU::GetCurrent();
}

namespace x64 {

CPU & CPU::GetCurrent() {
  CPU * address;
  __asm__("mov %%gs:(0x8), %0" : "=r" (address));
  return *address;
}

CPU::CPU(uint32_t _apicId) : apicId(_apicId) {
  AssertNoncritical();
  
  tss = new TSS();
  uint16_t sel = GDT::GetGlobal().AddTSS(tss);
  
  // initialize this CPU entry
  info.dedicatedStack = (void *)((new uint8_t[0x4000]) + 0x4000);
  tss->rsp[0] = (uint64_t)info.dedicatedStack;
  tss->ist[0] = tss->rsp[0];
  tssSelector = sel;

  info.thisPtr = this;
}
  
uint32_t CPU::GetId() {
  return apicId;
}

void * CPU::GetDedicatedStack() {
  return info.dedicatedStack;
}

TSS * CPU::GetTSS() {
  return tss;
}

uint64_t CPU::GetTSSSelector() {
  return tssSelector;
}

void CPU::LoadGS() {
  AssertCritical();
  GSRegister::SetBase((uint64_t)&info);
}

UserMap * CPU::GetCurrentMap() {
  return currentMap;
}

void CPU::SetCurrentMap(UserMap * map) {
  currentMap = map;
}

void CPU::SetKernelStack(void * top) {
  tss->rsp[0] = (uint64_t)top;
  info.threadKernStack = top;
}

void CPU::Wake() {
  AssertCritical();
  LAPIC & lapic = LAPIC::GetCurrent();
  lapic.SendIPI(GetId(), IntVectors::LapicTimer);
}

}

}

