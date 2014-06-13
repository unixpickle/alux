#ifndef __SCHEDULER_USER_TASK_HPP__
#define __SCHEDULER_USER_TASK_HPP__

#include <scheduler/general/task.hpp>
#include <scheduler/user/code-map.hpp>

namespace OS {

class UserTask : public Task {
public:
  typedef Task super;
  
  static UserTask * New(UserCode * code); // @noncritical
  virtual void Delete();
  virtual bool IsKernel();
  
  CodeMap & GetCodeMap();
  
protected:
  UserTask(UserCode * code);
  
  CodeMap codeMap;
};

}

#endif
