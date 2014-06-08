#include <arch/x64/smp/cpu.hpp>
#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/interrupts/vectors.hpp>
#include <utilities/critical.hpp>
#include <utilities/lock.hpp>

namespace OS {

namespace x64 {

CPU::CPU(uint32_t _apicId) : apicId(_apicId) {
}
  
uint32_t CPU::GetAPICID() {
  return apicId;
}

}

}
