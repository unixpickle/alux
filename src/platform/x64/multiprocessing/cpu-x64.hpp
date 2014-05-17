#ifndef __PLATFORM_X64_CPU_X64_H__
#define __PLATFORM_X64_CPU_X64_H__

#include <platform/multiprocessing.hpp>

namespace OS {

namespace x64 {

class CPU : public Processor {
private:
  int logicalCPUBits;
  int coreCPUBits;
  uint32_t apicId;

public:
  CPU(uint32_t apicId); // only call this from the current CPU

  int GetIndexInCore();
  int GetIndexInCPU();
};

}

}

#ifndef

