#ifndef __SCHEDULER_KERNEL_TASK_HPP__
#define __SCHEDULER_KERNEL_TASK_HPP__

#include <scheduler/general/task.hpp>

namespace OS {

class KernelTask : public Task {
public:
  static void Initialize(); // @noncritical
  static KernelTask & GetGlobal(); // @ambicritical
  
  KernelTask(); // DO NOT CALL THIS YOURSELF
  
  virtual void Delete(); // @noncritical, don't call
};

}

#endif
