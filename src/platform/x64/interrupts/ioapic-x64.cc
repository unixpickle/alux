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

#include "ioapic-x64.hpp"

namespace OS {

namespace x64 {

static IOAPIC baseAPIC;

void IOAPIC::StartUsing() {
  OutB(0x22, 0x70);
  OutB(0x23, 0x01);
}

IOAPIC::IOAPIC(ACPI::MADT::IOAPIC * info) : madtInfo(*info) {
  base = (PhysAddr)madtInfo.baseAddr;
  
  // map in the base address so we can access it from virtual memory
  VirtAddr addr;
  bool res = KernMap::Map(base, 0x1000, addr);
  assert(res);
  regs = (volatile uint32_t *)addr;
  
  cout << "IOAPIC::IOAPIC(" << (uintptr_t)info << ") - base=" << base
    << " version=" << GetVersion() << " pinCount=" << GetPinCount() << endl;
}

IOAPIC::~IOAPIC() {
  // unmap the address here
  Panic("Destroying an IOAPIC will result in a map leak.");
  // KernMap::Unmap((VirtAddr)regs, 0x1000);
}

void IOAPIC::WriteReg(uint8_t reg, uint32_t val) {
  regs[0] = reg;
  regs[4] = val;
}

uint32_t IOAPIC::ReadReg(uint8_t reg) {
  regs[0] = reg;
  return regs[4];
}

uint32_t IOAPIC::GetVersion() {
  return ReadReg(RegVersion) & 0xff;
}

uint32_t IOAPIC::GetPinCount() {
  return 1 + ((ReadReg(RegVersion) >> 0x10) & 0xff);
}

uint32_t IOAPIC::GetInterruptBase() {
  return madtInfo.interruptBase;
}

void IOAPIC::SetTable(uint8_t idx, const TableEntry & entry) {
  const uint32_t * valPtr = (const uint32_t *)&entry;
  WriteReg(0x10 + (idx * 2), 0x10000); // mask the entry
  WriteReg(0x11 + (idx * 2), valPtr[1]);
  WriteReg(0x10 + (idx * 2), valPtr[0]); 
}

void IOAPIC::MapIRQ(uint8_t irq, uint8_t vector) {
  TableEntry entry;
  entry.vector = vector;
  
  ACPI::MADT::ISO * iso = ACPI::GetMADT()->LookupISO(irq);
  if (iso) {
    if (iso->flags & 0x3) entry.intpol = 1;
    if ((iso->flags >> 2) & 0x3) entry.triggermode = 1;
    SetTable(iso->interrupt, entry);
  } else {
    SetTable(irq, entry);
  }
}

void IOAPIC::MaskPin(uint8_t irq) {
  WriteReg(0x10 + (irq * 2), 0x10000);
}

void InitializeIOAPIC() {
  ACPI::MADT * madt = ACPI::GetMADT();
  if (!madt) Panic("I/O APIC support requires MADT");
  cout << "MADT info: IOAPICs=" << madt->CountIOAPICEntries() << " LAPICS="
    << madt->CountLocalAPICEntries() << endl;
  
  ACPI::MADT::IOAPIC * info = madt->GetIOAPICWithBase(0);
  if (!info) Panic("No base I/O APIC found :(");
  
  new(&baseAPIC) IOAPIC(info);
}

IOAPIC & GetBaseIOAPIC() {
  return baseAPIC;
}

}

}
