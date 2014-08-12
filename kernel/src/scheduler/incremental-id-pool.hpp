#ifndef __ALUX_INCREMENTAL_ID_POOL_HPP__
#define __ALUX_INCREMENTAL_ID_POOL_HPP__

#include "task-id-pool.hpp"
#include "../tasks/task.hpp"
#include <anarch/lock>

namespace OS {

class IncrementalIdPool : public TaskIdPool {
public:
  virtual TaskId Alloc(Task & task);
  virtual void Free(Task & task);
  virtual Task * GetTask(TaskId ident);
  
private:
  anarch::NoncriticalLock lock;
  ansa::LinkedList<Task> tasks;
  TaskId step = 0;
};

}

#endif
