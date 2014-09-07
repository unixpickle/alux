#ifndef __ALUX_USER_TASK_HPP__
#define __ALUX_USER_TASK_HPP__

#include "task.hpp"
#include "../arch/all/executable.hpp"

namespace Alux {

/**
 * A task which runs a user-space [Executable].
 */
class UserTask : public Task {
public:
  /**
   * Allocate and construct a [UserTask]. If a task cannot be allocated, the
   * kernel will Panic().
   * @noncritical
   */
  static UserTask & New(Executable &, anarch::UserMap &, Identifier,
                        Scheduler &);
  
  /**
   * Returns the executable map for this task.
   * @ambicritical
   */
  inline ExecutableMap & GetExecutableMap() {
    return executableMap;
  }
  
  /**
   * Returns the task's memory map.
   * @ambicritical
   */
  virtual anarch::UserMap & GetMemoryMap();
  
  /**
   * Returns `true`.
   * @ambicritical
   */
  virtual bool IsUserTask();
  
  /**
   * Calls Task::Dealloc() and then deallocates the memory used by this task.
   * @noncritical
   */
  virtual void Dealloc();
  
private:
  UserTask(Executable &, anarch::UserMap &, Identifier, Scheduler &);
  virtual ~UserTask();
  
  anarch::UserMap & memoryMap;
  ExecutableMap & executableMap;
};

}

#endif
