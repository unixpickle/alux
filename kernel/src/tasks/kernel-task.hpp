#ifndef __ALUX_KERNEL_TASK_HPP__
#define __ALUX_KERNEL_TASK_HPP__

#include "task.hpp"

namespace Alux {

/**
 * A task that runs in the kernel's address space with kernel-level
 * permissions.
 */
class KernelTask : public Task {
public:
  /**
   * Allocate and initialize a [KernelTask]. If the allocation fails, the
   * kernel will kindly Panic().
   * @noncritical
   */
  static KernelTask * New(Scheduler &);
  
  /**
   * Returns the global memory map.
   * @ambicritical
   */
  virtual anarch::MemoryMap & GetMemoryMap();
  
  /**
   * Returns `false`.
   * @ambicritical
   */
  virtual bool IsUserTask();
  
  /**
   * Calls Task::Dealloc() and then deallocates the memory used by this task.
   * @noncritical
   */
  virtual void Dealloc();
  
protected:
  KernelTask(Scheduler &);
};

}

#endif
