#ifndef __ALUX_KERNEL_TASK_HPP__
#define __ALUX_KERNEL_TASK_HPP__

#include "task.hpp"

namespace Alux {

class KernelTask : public Task {
public:
  /**
   * Create a [KernelTask] that belongs to a given scheduler.
   *
   * If the task cannot be allocated or a PID cannot be generated for it,
   * `NULL` will be returned. Otherwise, the returned task will have a hold
   * count of 1 and a retain count of 0.
   *
   * @noncritical
   */
  static KernelTask * New(Scheduler &);
  
  /**
   * Returns the global memory map.
   * @ambicritical
   */
  virtual anarch::MemoryMap & GetMemoryMap() const;
  
  /**
   * Returns `false`.
   * @ambicritical
   */
  virtual bool IsUserTask() const;
  
  /**
   * Inverse of [New]. This may run some sort of SLAB deallocation, or it may
   * just `delete this`.
   * @noncritical
   */
  virtual void Dealloc();
  
protected:
  KernelTask(Scheduler &);
};

}

#endif
