#ifndef __X64_CPU_HPP__
#define __X64_CPU_HPP__

#include <arch/x64/segments/tss.hpp>
#include <arch/x64/vmm/tlb.hpp>
#include <arch/general/hardware-thread.hpp>

namespace OS {

namespace x64 {

class UserMap;

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

  UserMap * GetCurrentMap();
  void SetCurrentMap(UserMap * map);

  virtual void Wake();

protected:
  virtual void SetCurrentThread(Thread * th);

private:
  struct CriticalInformation {
    void * dedicatedStack; // top, not bottom
    CPU * thisPtr;
    void * threadKernStack;
  } OS_PACKED;

  UserMap * currentMap OS_ALIGNED(8);

  uint32_t apicId;
  uint16_t tssSelector;
  TSS * tss;
  
  CriticalInformation info;
};

}

}

#endif
