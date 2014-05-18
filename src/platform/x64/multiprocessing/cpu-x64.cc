#include "cpu-x64.hpp"

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

ProcessorID CPU::GetID() {
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

namespace CPUList {
  
  static uint8_t * listBuffer;
  static int fillCount = 0;
  static int maxCount;
  
  void Initialize(int _maxCount) {
    maxCount = _maxCount;
    listBuffer = new uint8_t[maxCount * sizeof(CPU)];
  }
  
  int Count() {
    return fillCount;
  }
  
  int ConstructEntry(uint32_t apicId) {
    assert(maxCount > fillCount);
    void * destPtr = (void *)(listBuffer + sizeof(CPU) * fillCount);
    new(destPtr) CPU(apicId);
    return fillCount++;
  }
  
  CPU & GetEntry(int index) {
    assert(index >= 0 && index < fillCount);
    CPU * ptr = (CPU *)listBuffer;
    return ptr[index];
  }
  
}

}

}
