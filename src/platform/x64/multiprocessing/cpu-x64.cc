#include "cpu-x64.hpp"
#include "../gdt-x64.hpp"

namespace OS {
namespace x64 {

CPU::CPU(uint32_t _apicId) : apicId(_apicId) {
  // TODO: use CPUID here
}

int CPU::GetIndexInCore() {
  // TODO: nyi
  return 0;
}

int CPU::GetIndexInCPU() {
  // TODO: nyi
  return 0;
}

ProcessorID CPU::GetId() {
  return (ProcessorID)apicId;
}

int CPU::GetPriority() {
  // TODO: return based on whether this is a hypercore or not
  return 0;
}

void * CPU::SendIPI(void * object) {
  (void)object;
  // TODO: nyi
  Panic("NYI");
  return NULL;
}

}
}
