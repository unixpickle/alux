#ifndef __X64_CPU_HPP__
#define __X64_CPU_HPP__

#include <arch/x64/segments/tss.hpp>
#include <arch/x64/smp/invlpg.hpp>
#include <arch/general/hardware-thread.hpp>

namespace OS {

namespace x64 {

class CPU : public HardwareThread {
public:
  static CPU & GetCurrent();

  struct {
    uint64_t lapic;
  } frequencies;
  
  // used for passing INVLPG requests
  uint64_t invlpgLock OS_ALIGNED(8); // @critical
  InvlpgInfo * invlpgInfo;
  
  CPU(uint32_t apicId);
    
  uint32_t GetId();
  void * GetDedicatedStack(); // @ambicritical
  TSS * GetTSS(); // @ambicritical
  uint64_t GetTSSSelector(); // @ambicritical
  void LoadGS(); // @critical

  virtual void Wake();

private:
  struct CriticalInformation {
    void * dedicatedStack; // top, not bottom
    CPU * thisPtr;
  } OS_PACKED;

  uint32_t apicId;
  uint16_t tssSelector;
  TSS * tss;
  CriticalInformation info;
};

}

}

#endif
