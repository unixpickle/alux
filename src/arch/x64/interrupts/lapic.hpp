#ifndef __X64_LAPIC_HPP__
#define __X64_LAPIC_HPP__

#include <cstdint>
#include <memory/easy-map.hpp>

namespace OS {

namespace x64 {

class LAPIC {
public:
  /**
   * During the boot process, this must only be called on one CPU at a time.
   * @critical
   */
  static LAPIC & GetCurrent();

  virtual ~LAPIC();
  virtual void SetDefaults(); // @critical
  virtual void Enable() = 0; // @critical
  virtual uint32_t GetId() = 0; // @critical
  virtual void ClearErrors(); // @critical

  virtual void SendEOI(); // @critical
  virtual void SetPriority(uint8_t vector);
  virtual bool IsRequested(uint8_t vector); // @critical
  virtual bool IsInService(uint8_t vector); // @critical

  /**
   * @critical
   */
  virtual void SendIPI(uint32_t cpu, uint8_t vector,
                       uint8_t mode = 0, uint8_t level = 1,
                       uint8_t trigger = 0) = 0;

  virtual uint64_t ReadReg(uint16_t reg) = 0; // @critical
  virtual void WriteReg(uint16_t reg, uint64_t value) = 0; // @critical
  
  enum Regs {
    RegApicId = 0x2,
    RegApicVer = 0x3,
    RegTaskPriority = 0x8,
    RegEOI = 0xb,
    RegSpurious = 0xf,
    RegESR = 0x28,
    RegICR = 0x30,
    RegLVTTimer = 0x32,
    RegLVTPerf = 0x34,
    RegLVTLint0 = 0x35,
    RegLVTLint1 = 0x36,
    RegLVTError = 0x37,
    RegTimerInitCount = 0x38,
    RegTimerCurrCount = 0x39,
    RegTimerDiv = 0x3e
  };
};

class XAPIC : public LAPIC {
private:
  EasyMap * map;
  uint64_t base;

public:
  XAPIC(uint64_t base);
  virtual ~XAPIC();
  virtual uint64_t ReadReg(uint16_t reg);
  virtual void WriteReg(uint16_t reg, uint64_t value);
  virtual void Enable();
  virtual uint32_t GetId();
  virtual void SendIPI(uint32_t cpu, uint8_t vector,
                       uint8_t mode, uint8_t level,
                       uint8_t trigger);
};

class X2APIC : public LAPIC {
public:
  virtual uint64_t ReadReg(uint16_t reg);
  virtual void WriteReg(uint16_t reg, uint64_t value);
  virtual void Enable();
  virtual uint32_t GetId();
  virtual void SendIPI(uint32_t cpu, uint8_t vector,
                       uint8_t mode, uint8_t level,
                       uint8_t trigger);
};

}

}

#endif
