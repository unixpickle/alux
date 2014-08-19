#ifndef __ALUX_USER_TASK_HPP__
#define __ALUX_USER_TASK_HPP__

#include "task.hpp"
#include "../arch/all/app-code-map.hpp"
#include <anarch/api/user-map>

namespace OS {

class UserTask : public Task {
public:
  static UserTask & New(Scheduler & s, AppCodeMap & map);
  
  AppCodeMap & GetAppCodeMap() const;
  virtual anarch::MemoryMap & GetMemoryMap() const;
  virtual bool IsUserTask() const;
  
protected:
  UserTask(AppCodeMap &, UserMap &);
  virtual void Destroy();
  
private:
  AppCodeMap & codeMap;
  UserMap & map;
};

}

#endif
