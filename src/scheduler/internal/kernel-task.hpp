#ifndef __SCHEDULER_KERNEL_TASK_HPP__
#define __SCHEDULER_KERNEL_TASK_HPP__

#include <module/module.hpp>
#include <scheduler/general/task.hpp>

namespace OS {

class KernelTask : public Module {
public:
  static void InitGlobal(); // @noncritical
  static KernelTask & GetGlobal(); // @ambicritical
  
  Task * GetTask();
  
protected:
  virtual void Initialize();
  virtual DepList GetDependencies();
  virtual DepList GetSuperDependencies();
  
private:
  Task * task;
};

}

#endif
