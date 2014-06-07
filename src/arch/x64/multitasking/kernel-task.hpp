#ifndef __X64_KERNEL_TASK_HPP__
#define __X64_KERNEL_TASK_HPP__

#include <arch/x64/multitasking/task.hpp>

namespace OS {

namespace x64 {

class KernelTask : public Task {
public:
  typedef Task super;
  
  KernelTask(OS::TaskGroup * g, uint64_t method, uint64_t argument);
  virtual ~KernelTask();
  virtual void Run();

protected:
  bool hasRun;
  uint64_t argument;
  void * stack;
};

}

}

#endif
