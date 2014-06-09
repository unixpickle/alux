#ifndef __X64_THREAD_HPP__
#define __X64_THREAD_HPP__

#include <scheduler/base/thread.hpp>
#include <arch/x64/scheduler/task.hpp>

namespace OS {

namespace x64 {

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
};

class Thread : public OS::Thread {
public:
  typedef OS::Thread super;
  
  RegisterState state;
  uint8_t * kernStack;
  
  static Thread * New(Task * task, bool forKernel = false);
  
  Thread(Task * task, bool forKernel = false);
  virtual ~Thread();
  virtual void Delete();
  void SetEntry(uint64_t rip, uint64_t rdi);
  
  virtual void Run();
};

}

}

#endif
