#ifndef __SCHEDULER_USER_TASK_HPP__
#define __SCHEDULER_USER_TASK_HPP__

#include <scheduler/general/task.hpp>

namespace OS {

class UserTask : public Task {
public:
  static UserTask * New(); // @noncritical
  virtual void Delete();
  virtual bool IsKernel();
};

}

#endif
