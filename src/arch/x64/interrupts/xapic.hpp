#ifndef __X64_XAPIC_HPP__
#define __X64_XAPIC_HPP__

#include <arch/x64/interrupts/lapic.hpp>
#include <memory/easy-map.hpp>

namespace OS {

namespace x64 {

class XAPIC : public LAPIC {
private:
  EasyMap * map;
  uint64_t base;

public:
  XAPIC(); // do not call this
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

}

}

#endif
