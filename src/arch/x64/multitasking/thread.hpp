#ifndef __X64_THREAD_HPP__
#define __X64_THREAD_HPP__

#include <multitasking/thread.hpp>
#include <cstdint>
#include <common>

namespace OS {

namespace x64 {

class Thread : public OS::Thread {
public:
  typedef OS::Thread super;
  
  class Registers {
  public:
    // these are ordered this way for iretq
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
    
    uint64_t cr3;
  } OS_PACKED;
  
  Registers regs;
  
  Thread(OS::Task * g);
  virtual ~Thread();
  virtual void Run();
  
protected:
  void * kernStack;
  void SetStackInCPU();
};

}

}

#endif
