#ifndef __X64_KERNEL_THREAD_HPP__
#define __X64_KERNEL_THREAD_HPP__

#include <arch/x64/multitasking/thread.hpp>

namespace OS {

namespace x64 {

class KernelThread : public Thread {
public:
  typedef Thread super;
  
  KernelThread(OS::Task * g, uint64_t method, uint64_t argument);
  virtual ~KernelThread();
  virtual void Run();

protected:
  bool hasRun;
  uint64_t argument;
  void * stack;
};

}

}

#endif
