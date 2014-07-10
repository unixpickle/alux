#include <arch/x64/segments/gs-register.hpp>
#include <arch/x64/common.hpp>
#include <critical>

namespace OS {

namespace x64 {

void GSRegister::SetBase(uint64_t index) {
  AssertCritical();
  WriteMSR(0xc0000101, index);
}

uint64_t GSRegister::GetBase() {
  AssertCritical();
  return ReadMSR(0xc0000101);
}

bool GSRegister::ShouldSwap(uint64_t cs, uint64_t vector) {
  AssertCritical();
  if (vector != 0x2 && vector != 0x12) {
    return (cs & 3) != 0;
  } else {
    return GetBase() == 0;
  }
}

void GSRegister::Swap() {
  AssertCritical();
  __asm__ __volatile__("swapgs" : : : "memory");
}

ScopeSwapGS::ScopeSwapGS(uint64_t cs, uint64_t vector) {
  AssertCritical();
  shouldSwap = GSRegister::ShouldSwap(cs, vector);
  if (shouldSwap) GSRegister::Swap();
}

ScopeSwapGS::~ScopeSwapGS() {
  AssertCritical();
  if (shouldSwap) GSRegister::Swap();
}

}

}
