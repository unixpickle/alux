#ifndef __X64_CPU_HPP__
#define __X64_CPU_HPP__

#include <arch/x64/segments/tss.hpp>
#include <arch/x64/smp/invlpg.hpp>
#include <arch/general/cpu.hpp>

namespace OS {

namespace x64 {

class CPU : public OS::CPU {
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
  
  virtual size_t GetIndex();
  virtual void Wakeup();

  Task * GetTask();
  void SetTask(Task * task);

private:
  uint32_t apicId;
  Task * task OS_ALIGNED(8);
  
};

}

}

#endif
