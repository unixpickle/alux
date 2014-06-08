#ifndef __MULTITASKING_KERNEL_TASK_HPP__
#define __MULTITASKING_KERNEL_TASK_HPP__

#include <multitasking/garbage-thread.hpp>
#include <multitasking/task.hpp>

namespace OS {

class KernelTask {
public:
  static void Initialize();
  static KernelTask & GetGlobal();
  
  KernelTask();
  
  GarbageThread & GetGarbageThread();
  
protected:
  Task * task;
  GarbageThread garbage;
};

}

#endif
