#ifndef __PLATFORM_X64_LAPIC_X64_H__
#define __PLATFORM_X64_LAPIC_X64_H__

#include "../acpi/madt-x64.hpp"
#include "../common-x64.hpp"

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
  virtual void SetDefaults(); // @critical
  virtual void Enable() = 0; // @critical
  virtual uint32_t GetId() = 0; // @critical
  virtual void ClearErrors(); // @critical

  virtual void SendEOI(); // @critical
  virtual bool IsRequested(uint8_t vector); // @critical
  virtual bool IsInService(uint8_t vector); // @critical

  /**
   * @critical
   */
  virtual void SendIPI(uint32_t cpu, uint8_t vector,
                       uint8_t mode, uint8_t level,
                       uint8_t trigger) = 0;

  virtual uint64_t ReadRegister(uint16_t reg) = 0; // @critical
  virtual void WriteRegister(uint16_t reg, uint64_t value) = 0; // @critical
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

/**
 * @critical
 */
LAPIC & GetLocalAPIC();

}

}

#endif
