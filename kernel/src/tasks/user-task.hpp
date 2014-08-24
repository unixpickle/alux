#ifndef __ALUX_USER_TASK_HPP__
#define __ALUX_USER_TASK_HPP__

#include "task.hpp"
#include "../arch/all/executable.hpp"

namespace Alux {

class UserTask : public Task {
public:
  /**
   * Create a [UserTask] that belongs to a given scheduler.
   *
   * If the task cannot be allocated or a PID cannot be generated for it,
   * `NULL` will be returned. Otherwise, the returned task will have a hold
   * count of 1 and a retain count of 0.
   *
   * @noncritical
   */
  UserTask * New(Executable &, anarch::UserMap &, Scheduler &);
  
  /**
   * Returns the task's memory map.
   * @ambicritical
   */
  virtual anarch::UserMap & GetMemoryMap() const;
  
  /**
   * Returns `true`.
   * @ambicritical
   */
  virtual bool IsUserTask() const;
  
  /**
   * Inverse of [New]. This may run some sort of SLAB deallocation, or it may
   * just `delete this`.
   * @noncritical
   */
  virtual void Dealloc();
  
private:
  UserTask(Executable &, anarch::UserMap &, Scheduler &);
  virtual ~UserTask();
  
  anarch::UserMap & memoryMap;
  ExecutableMap & executableMap;
};

}

#endif
