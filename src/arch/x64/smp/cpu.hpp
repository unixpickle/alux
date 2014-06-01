#ifndef __X64_CPU_HPP__
#define __X64_CPU_HPP__

#include <cstdint>
#include <arch/x64/segments/tss.hpp>

namespace OS {

namespace x64 {

class CPU {
public:
  uint16_t tssSelector;
  TSS * tss;
  void * dedicatedStack; // top of the stack, not bottom of it
  struct {
    uint64_t lapic;
  } frequencies;
  
  CPU(uint32_t apicId);
    
  uint32_t GetAPICID();

private:
  uint32_t apicId;
};

}

}

#endif
