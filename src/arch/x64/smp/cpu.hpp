#ifndef __X64_CPU_HPP__
#define __X64_CPU_HPP__

#include <arch/x64/segments/tss.hpp>
#include <arch/x64/smp/invlpg.hpp>
#include <arch/general/hardware-thread.hpp>

namespace OS {

namespace x64 {

class CPU : public HardwareThread {
public:
  uint16_t tssSelector;
  TSS * tss;
  void * dedicatedStack; // top of the stack, not bottom of it
  struct {
    uint64_t lapic;
  } frequencies;
  
  uint64_t invlpgLock OS_ALIGNED(8); // @critical
  InvlpgInfo * invlpgInfo;
  
  CPU(uint32_t apicId);
    
  uint32_t GetAPICID();
  virtual int GetIndex();
  virtual void Wake();

private:
  uint32_t apicId;
  
};

}

}

#endif
