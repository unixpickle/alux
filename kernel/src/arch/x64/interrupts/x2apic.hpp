#ifndef __X64_X2APIC_HPP__
#define __X64_X2APIC_HPP__

#include <arch/x64/interrupts/lapic.hpp>

namespace OS {

namespace x64 {

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
