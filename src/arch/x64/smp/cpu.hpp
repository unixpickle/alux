#ifndef __X64_CPU_HPP__
#define __X64_CPU_HPP__

#include <cstdint>

namespace OS {

namespace x64 {

class CPU {
public:
  uint16_t tss;
  void * dedicatedStack; // top of the stack, not bottom of it
  
  CPU(uint32_t apicId);
    
  uint32_t GetAPICID();

private:
  uint32_t apicId;
};

}

}

#endif
