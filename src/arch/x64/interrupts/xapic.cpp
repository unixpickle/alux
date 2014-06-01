#include <arch/x64/interrupts/xapic.hpp>
#include <arch/x64/common.hpp>
#include <arch/general/failure.hpp>
#include <utilities/critical.hpp>

namespace OS {

namespace x64 {

XAPIC::XAPIC() {
  Panic("XAPIC::XAPIC() - for the compiler only");
}

XAPIC::XAPIC(uint64_t _base) : base(_base) {
  map = new EasyMap(base, 0x1000);
}

XAPIC::~XAPIC() {
  delete map;
}

uint64_t XAPIC::ReadReg(uint16_t reg) {
  AssertCritical();
  volatile uint32_t * regs = (volatile uint32_t *)map->GetStart();
  if (reg != 0x30) {
    return (uint64_t)regs[reg * 4];
  } else {
    uint32_t lower = regs[reg * 4];
    uint32_t higher = regs[(reg + 1) * 4];
    return (uint64_t)lower | ((uint64_t)higher << 0x20);
  }
}

void XAPIC::WriteReg(uint16_t reg, uint64_t value) {
  AssertCritical();
  volatile uint32_t * regs = (volatile uint32_t *)map->GetStart();
  if (reg != 0x30) {
    assert(!(value & 0xFFFFFFFF00000000L));
    regs[reg * 4] = value;
  } else {
    regs[(reg + 1) * 4] = value >> 0x20;
    regs[reg * 4] = value & 0xFFFFFFFF;
  }
}

void XAPIC::Enable() {
  AssertCritical();
  uint64_t flags = ReadMSR(0x1b) & 0xf00;
  flags |= 1 << 11;
  WriteMSR(0x1b, base | flags);
}

uint32_t XAPIC::GetId() {
  AssertCritical();
  return ReadReg(RegApicId) >> 0x18;
}

void XAPIC::SendIPI(uint32_t cpu, uint8_t vector,
                    uint8_t mode, uint8_t level,
                    uint8_t trigger) {
  AssertCritical();
  uint64_t value = 0;
  value = (uint64_t)vector | ((uint64_t)mode << 8);
  value |= ((uint64_t)level << 0xe) | ((uint64_t)trigger << 0xf);
  value |= ((uint64_t)cpu << 0x38);
  WriteReg(RegICR, value);
}

}

}
