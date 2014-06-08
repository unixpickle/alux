#ifndef __SCHEDULER_TYPES_HPP__
#define __SCHEDULER_TYPES_HPP__

#include <scheduler/job-group.hpp>

namespace OS {

namespace Scheduler {

class Job : public Destructable {
public:
  UserInfo * userInfo;
  
  virtual ~Job() {
    if (userInfo) delete userInfo;
  }
  
  virtual void Run() = 0; // @critical (no return)
  virtual JobGroup * GetJobGroup() = 0; // @ambicritical
};

}

}

#endif
