#ifndef __X64_THREAD_HPP__
#define __X64_THREAD_HPP__

#include <cstdint>
#include <cstddef>
#include <common>

namespace OS {

class Task;

namespace x64 {

class ArchThread {
public:
  class RegisterState {
  public:
    // these are ordered this way for iretq
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
  
    uint64_t rdi;
    uint64_t cr3;
  } OS_PACKED;
  
  RegisterState state;
  uint8_t * kernStack;
  
  ArchThread(Task * task, bool forKernel = false);
  ~ArchThread();

  void SetKernelCall(void * function, void * argument = NULL);
  void Run();
};

}

typedef x64::ArchThread ArchThread;

}

#endif
