#ifndef __ALUX_KERNEL_TASK_HPP__
#define __ALUX_KERNEL_TASK_HPP__

#include "task.hpp"

namespace OS {

class KernelTask : public Task {
public:
  static KernelTask & New(Scheduler &);
  
  virtual anarch::MemoryMap & GetMemoryMap() const;
  virtual bool IsUserTask() const;
  
protected:
  KernelTask(Scheduler &);
  
  virtual void Destroy();
};

}

#endif
