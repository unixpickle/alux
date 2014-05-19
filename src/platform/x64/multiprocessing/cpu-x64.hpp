#ifndef __PLATFORM_X64_CPU_X64_HPP__
#define __PLATFORM_X64_CPU_X64_HPP__

#include <platform/multiprocessing.hpp>
#include <platform/failure.hpp>
#include <cassert>
#include <new>

namespace OS {

namespace x64 {

class CPU : public Processor {
private:
  int logicalCPUBits;
  int coreCPUBits;

public:
  uint32_t apicId;
  uint16_t tssDesc;
  void * stack;
  
  uint64_t apicClockSpeed;
  bool hasCalibrated;
  
  CPU(uint32_t apicId); // only call this from the current CPU

  int GetIndexInCore();
  int GetIndexInCPU();
  
  virtual ProcessorID GetId();
  virtual int GetPriority();
  virtual void * SendIPI(void * object);
};

namespace CPUList {
  
  void Initialize(int maxCount);
  int GetCount();
  int ConstructEntry(uint32_t apicId);
  CPU & GetEntry(int index);
  
}

}

}

#endif
