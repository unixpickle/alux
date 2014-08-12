#ifndef __ALUX_TASK_Id_POOL_HPP__
#define __ALUX_TASK_Id_POOL_HPP__

#include "../tasks/task.hpp"

namespace OS {

/**
 * An abstract process identifier allocation pool.
 */
class TaskIdPool {
public:  
  /**
   * Allocate a new task Id, assign it to a task, and return it.
   * @noncritical
   */
  virtual TaskId Alloc(Task &) = 0;
  
  /**
   * Return a task's Id to the pool.
   * @noncritical
   */
  virtual void Free(Task &) = 0;
  
  /**
   * Lookup a task by id, retain it, and return it. If the task could not be
   * found or retained, `NULL` will be returned.
   * @noncritical
   */
  virtual Task * GetTask(TaskId) = 0;
};

}

#endif
