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

#ifndef __PLATFORM_X64_LAPIC_X64_H__
#define __PLATFORM_X64_LAPIC_X64_H__

#include "../acpi/madt-x64.h"
#include "../common-x64.h"

namespace OS {

namespace x64 {

class LAPIC {
public:
  static const int RegAPICID = 0x2;
  static const int RegAPICVER = 0x3;
  static const int RegTASKPRIOR = 0x8;
  static const int RegEOI = 0xb;
  static const int RegLDR = 0xd;
  static const int RegDFR = 0xe;
  static const int RegSPURIOUS = 0xf;
  static const int RegESR = 0x28;
  static const int RegICRL = 0x30;
  static const int RegICRH = 0x31;
  static const int RegLVT_TMR = 0x32;
  static const int RegLVT_PERF = 0x34;
  static const int RegLVT_LINT0 = 0x35;
  static const int RegLVT_LINT1 = 0x36;
  static const int RegLVT_ERR = 0x37;
  static const int RegTMRINITCNT = 0x38;
  static const int RegTMRCURRCNT = 0x39;
  static const int RegTMRDIV = 0x3e;

  static const int SettingLAST = 0x38f;
  static const int SettingSW_ENABLE = 0x100;
  static const int SettingCPUFOCUS = 0x200;
  static const int SettingDISABLE = 0x1000;
  static const int SettingNMI = (1 << 10);
  static const int SettingTMR_PERIODIC = 0x20000;
  static const int SettingTMR_BASEDIV = (1 << 20);

  virtual ~LAPIC();
  virtual void SetDefaults();
  virtual void Enable() = 0;
  virtual uint32_t GetId() = 0;
  virtual void ClearErrors();

  virtual void SendEOI();
  virtual bool IsRequested(uint8_t vector);
  virtual bool IsInService(uint8_t vector);

  virtual void SendIPI(uint32_t cpu, uint8_t vector,
                       uint8_t mode, uint8_t level,
                       uint8_t trigger) = 0;

  virtual uint64_t ReadRegister(uint16_t reg) = 0;
  virtual void WriteRegister(uint16_t reg, uint64_t value) = 0;
};

class XAPIC : public LAPIC {
private:
  uint64_t base;
  volatile uint32_t * regs;

public:
  XAPIC(uint64_t _base);
  virtual ~XAPIC();
  virtual uint64_t ReadRegister(uint16_t reg);
  virtual void WriteRegister(uint16_t reg, uint64_t value);
  virtual void Enable();
  virtual uint32_t GetId();
  virtual void SendIPI(uint32_t cpu, uint8_t vector,
                       uint8_t mode, uint8_t level,
                       uint8_t trigger);
};

class X2APIC : public LAPIC {
public:
  virtual uint64_t ReadRegister(uint16_t reg);
  virtual void WriteRegister(uint16_t reg, uint64_t value);
  virtual void Enable();
  virtual uint32_t GetId();
  virtual void SendIPI(uint32_t cpu, uint8_t vector,
                       uint8_t mode, uint8_t level,
                       uint8_t trigger);
};

void InitializeLocalAPIC();
LAPIC & GetLocalAPIC();

}

}

#endif
