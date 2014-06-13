#include <arch/x64/interrupts/xapic.hpp>
#include <arch/x64/interrupts/x2apic.hpp>
#include <arch/x64/acpi/madt.hpp>
#include <arch/x64/common.hpp>
#include <critical>
#include <cassert>
#include <new>

namespace OS {

namespace x64 {

static XAPIC xapic;
static X2APIC x2apic;
static int PreferredDivide = 0x3;

void LAPIC::Initialize() {
  MADT * madt = MADT::GetGlobal();
  assert(madt != NULL);
  new(&xapic) XAPIC((uint64_t)madt->GetHeader().lapicAddr);
  new(&x2apic) X2APIC();
}

LAPIC & LAPIC::GetCurrent() {
  AssertCritical();
  
  uint32_t ecx;
  CPUID(1, NULL, &ecx, NULL);
  if (ecx & (1 << 21)) {
    return x2apic;
  } else {
    return xapic;
  }
}

LAPIC::~LAPIC() {
}

void LAPIC::SetDefaults() {
  AssertCritical();
  WriteReg(RegTaskPriority, 0x0);
  WriteReg(RegLVTTimer, 0x10000);
  WriteReg(RegLVTPerf, 0x10000);
  WriteReg(RegLVTLint0, 0x8700);
  WriteReg(RegLVTLint1, 0x400);
  WriteReg(RegLVTError, 0x10000);
  WriteReg(RegSpurious, 0x1ff);

  // reset might have shut them off
  WriteReg(RegLVTLint0, 0x8700);
  WriteReg(RegLVTLint1, 0x400);

  WriteReg(RegTimerDiv, PreferredDivide);
}

void LAPIC::ClearErrors() {
  AssertCritical();
  WriteReg(RegESR, 0);
}

void LAPIC::SendEOI() {
  AssertCritical();
  WriteReg(RegEOI, 0);
}

void LAPIC::SetPriority(uint8_t vector) {
  AssertCritical();
  WriteReg(RegTaskPriority, vector);
}

bool LAPIC::IsRequested(uint8_t vector) {
  AssertCritical();
  uint64_t regIndex = 0x20 + (vector >> 5);
  uint32_t mask = (1 << (vector & 0x1f));
  return 0 != (ReadReg(regIndex) & mask);
}

bool LAPIC::IsInService(uint8_t vector) {
  AssertCritical();
  uint64_t regIndex = 0x10 + (vector >> 5);
  uint32_t mask = (1 << (vector & 0x1f));
  return 0 != (ReadReg(regIndex) & mask);
}

void LAPIC::SetTimeout(uint8_t vector, uint32_t ticks) {
  uint32_t timerField = vector;// | (2 << 17); // mode is bit 17
  WriteReg(RegLVTTimer, timerField);
  WriteReg(RegTimerInitCount, ticks);
}

void LAPIC::ClearTimeout() {
  WriteReg(RegLVTTimer, 0x10000);
}

}

}
