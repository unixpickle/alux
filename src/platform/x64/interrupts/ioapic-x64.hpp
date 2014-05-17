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

#ifndef __PLATFORM_X64_IOAPIC_X64_H__
#define __PLATFORM_X64_IOAPIC_X64_H__

#include "../acpi/madt-x64.hpp"
#include "../common-x64.hpp"
#include <platform/failure.hpp>
#include <platform/memory.hpp>

namespace OS {

namespace x64 {

class IOAPIC {
private:
  PhysAddr base;
  ACPI::MADT::IOAPIC madtInfo;
  
  volatile uint32_t * regs;
  
public:
  static const int RegVersion = 0x1;
  
  static void StartUsing();
  
  class TableEntry {
  public:
    unsigned vector : 8; // RW - processor register
    unsigned delmode : 3; // RW
    unsigned destmode : 1; // RW - determines type for destfield
    unsigned delstatus : 1; // RO
    unsigned intpol : 1; // RW - 0 = high active, 1 = low active
    unsigned remirr : 1; // RO
    unsigned triggermode : 1; // 1 = level sensitive, 0 = edge sensitive
    unsigned imask : 1; // 1 = prevent this interrupt
    unsigned long long reserved : 39; // set this to 0
    unsigned destfield : 8; // RW - APIC ID or "set of processors"
    
    TableEntry() : vector(0), delmode(0), destmode(0), delstatus(0), intpol(0),
      remirr(0), triggermode(0), imask(0), reserved(0), destfield(0) {}
  } OS_PACKED;
  
  IOAPIC() {
    Panic("This is only for the compiler's satisfaction!");
  }
  
  IOAPIC(ACPI::MADT::IOAPIC * _info);
  ~IOAPIC();
  
  void WriteReg(uint8_t reg, uint32_t val);
  uint32_t ReadReg(uint8_t reg);
  
  uint32_t GetVersion();
  uint32_t GetPinCount();
  uint32_t GetInterruptBase();
  
  void SetTable(uint8_t idx, const TableEntry & entry);
  void MapIRQ(uint8_t irq, uint8_t vector);
  void MaskPin(uint8_t irq);
};

void InitializeIOAPIC();
IOAPIC & GetBaseIOAPIC();

}

}

#endif
