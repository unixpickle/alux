#include <arch/x64/interrupts/x2apic.hpp>
#include <arch/x64/common.hpp>
#include <arch/general/failure.hpp>
#include <utilities/critical.hpp>

namespace OS {

namespace x64 {

uint64_t X2APIC::ReadReg(uint16_t reg) {
  AssertCritical();
  return ReadMSR((uint32_t)reg + 0x800);
}

void X2APIC::WriteReg(uint16_t reg, uint64_t value) {
  AssertCritical();
  WriteMSR((uint32_t)reg + 0x800, value);
}

void X2APIC::Enable() {
  AssertCritical();
  uint64_t flags = ReadMSR(0x1b) & 0xf00;
  flags |= 3 << 10;
  WriteMSR(0x1b, flags);
}

uint32_t X2APIC::GetId() {
  AssertCritical();
  return ReadReg(RegApicId);
}

void X2APIC::SendIPI(uint32_t cpu, uint8_t vector,
                     uint8_t mode, uint8_t level,
                     uint8_t trigger) {
  AssertCritical();
  uint64_t value = 0;
  value = (uint64_t)vector | ((uint64_t)mode << 8);
  value |= ((uint64_t)level << 0xe) | ((uint64_t)trigger << 0xf);
  value |= ((uint64_t)cpu << 0x20);
  WriteReg(RegICR, value);
}

}

}
