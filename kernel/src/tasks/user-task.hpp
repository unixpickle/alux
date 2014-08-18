#ifndef __ALUX_USER_TASK_HPP__
#define __ALUX_USER_TASK_HPP__

#include "task.hpp"
#include "../arch/all/user-program-map.hpp"

namespace OS {

class UserTask : public Task {
public:
  static UserTask & New(Scheduler & s, UserProgramMap & map);
  
  UserProgramMap & GetProgramMap() const;
  virtual anarch::MemoryMap & GetMemoryMap() const;
  virtual bool IsUserTask() const;
  
private:
  UserProgramMap & programMap;
  
};

}

#endif
