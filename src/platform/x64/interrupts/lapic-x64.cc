/*
 * Copyright (c) 2014, Alex Nichol and Alux contributors.
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "lapic-x64.h"

namespace OS {

namespace x64 {

static LAPIC * lapic = NULL;
static int LAPICDivide = 0x3;

LAPIC::~LAPIC() {
}

void LAPIC::SetDefaults() {
  WriteRegister(RegTASKPRIOR, 0x20);
  WriteRegister(RegLVT_TMR, 0x10000);
  WriteRegister(RegLVT_PERF, 0x10000);
  WriteRegister(RegLVT_LINT0, 0x8700);
  WriteRegister(RegLVT_LINT1, 0x400);
  WriteRegister(RegLVT_ERR, 0x10000);
  WriteRegister(RegSPURIOUS, 0x1ff);

  // reset might have shut them off
  WriteRegister(RegLVT_LINT0, 0x8700);
  WriteRegister(RegLVT_LINT1, 0x400);

  WriteRegister(RegTMRDIV, LAPICDivide);
}

void LAPIC::ClearErrors() {
  WriteRegister(RegESR, 0);
}

void LAPIC::SendEOI() {
  WriteRegister(RegEOI, 0);
}

bool LAPIC::IsRequested(uint8_t vector) {
  uint64_t regIndex = 0x20 + (vector >> 5);
  uint32_t mask = (1 << (vector & 0x1f));
  return 0 != (ReadRegister(regIndex) & mask);
}

bool LAPIC::IsInService(uint8_t vector) {
  uint64_t regIndex = 0x10 + (vector >> 5);
  uint32_t mask = (1 << (vector & 0x1f));
  return 0 != (ReadRegister(regIndex) & mask);
}

XAPIC::XAPIC(uint64_t _base) : base(_base) {
  VirtAddr addr;
  bool res = KernMap::Map(base, 0x1000, addr);
  assert(res);
  regs = (volatile uint32_t *)addr;
}

XAPIC::~XAPIC() {
  Panic("Destroying an XAPIC will cause a map leak");
  // KernMap::Unmap((VirtAddr)regs, 0x1000);
}

uint64_t XAPIC::ReadRegister(uint16_t reg) {
  if (reg != 0x30) {
    return (uint64_t)regs[reg * 4];
  } else {
    uint32_t lower = regs[reg * 4];
    uint32_t higher = regs[(reg + 1) * 4];
    return (uint64_t)lower | ((uint64_t)higher << 0x20);
  }
}

void XAPIC::WriteRegister(uint16_t reg, uint64_t value) {
  if (reg != 0x30) {
    assert(!(value & 0xFFFFFFFF00000000L));
    regs[reg * 4] = value;
  } else {
    regs[reg * 4] = value & 0xFFFFFFFF;
    regs[(reg + 1) * 4] = value >> 0x20;
  }
}

void XAPIC::Enable() {
  uint64_t flags = ReadMSR(0x1b) & 0xf00;
  flags |= 1 << 11;
  WriteMSR(0x1b, base | flags);
}

uint32_t XAPIC::GetId() {
  return ReadRegister(RegAPICID) >> 0x18;
}

void XAPIC::SendIPI(uint32_t cpu, uint8_t vector,
                    uint8_t mode, uint8_t level,
                    uint8_t trigger) {
  uint64_t value = 0;
  value = (uint64_t)vector | ((uint64_t)mode << 8);
  value |= ((uint64_t)level << 0xe) | ((uint64_t)trigger << 0xf);
  value |= ((uint64_t)cpu << 0x38);
  WriteRegister(0x30, value);
}

uint64_t X2APIC::ReadRegister(uint16_t reg) {
  return ReadMSR((uint32_t)reg + 0x800);
}

void X2APIC::WriteRegister(uint16_t reg, uint64_t value) {
  WriteMSR((uint32_t)reg + 0x800, value);
}

void X2APIC::Enable() {
  uint64_t flags = ReadMSR(0x1b) & 0xf00;
  flags |= 3 << 10;
  WriteMSR(0x1b, flags);
}

uint32_t X2APIC::GetId() {
  return ReadRegister(RegAPICID);
}

void X2APIC::SendIPI(uint32_t cpu, uint8_t vector,
                     uint8_t mode, uint8_t level,
                     uint8_t trigger) {
  uint64_t value = 0;
  value = (uint64_t)vector | ((uint64_t)mode << 8);
  value |= ((uint64_t)level << 0xe) | ((uint64_t)trigger << 0xf);
  value |= ((uint64_t)cpu << 0x20);
  WriteRegister(0x30, value);
}

void InitializeLocalAPIC() {
  uint32_t ecx;
  CPUID(1, NULL, &ecx, NULL);
  if (ecx & (1 << 21)) {
    // setup x2APIC
    lapic = new X2APIC();
  } else {
    // setup xAPIC
    ACPI::MADT * madt = ACPI::GetMADT();
    lapic = new XAPIC((uint64_t)madt->GetHeader().lapicAddr);
  }
}

LAPIC & GetLocalAPIC() {
  return *lapic;
}

}

}

