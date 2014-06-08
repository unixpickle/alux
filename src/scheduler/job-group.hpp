#ifndef __SCHEDULER_JOB_GROUP_HPP__
#define __SCHEDULER_JOB_GROUP_HPP__

#include <scheduler/user-info.hpp>

namespace OS {

namespace Scheduler {

class JobGroup : public Destructable {
public:
  UserInfo * userInfo;
  
  virtual ~JobGroup() {
    if (userInfo) delete userInfo;
  }
  
};

}

}

#endif
