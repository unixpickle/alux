#include <arch/x64/cpu.hpp>

namespace OS {

namespace x64 {

CPU::CPU(uint32_t _apicId) : apicId(_apicId) {
}
  
uint32_t CPU::GetAPICID() {
  return apicId;
}

}

}
