#ifndef __SCHEDULER_TASK_HPP__
#define __SCHEDULER_TASK_HPP__

#include <utilities/index-set.hpp>
#include <cstdint>
#include <common>

namespace OS {

class Thread;

enum KillReason {
  KillReasonSelf = 0,
  KillReasonExternal,
  KillReasonMemory
};

class Task {
public:
  Task * next, * last;
  
  /**
   * This must be implemented by the specific platform to return either a Task
   * or some subclass of Task with specific information.
   *
   * @noncritical
   */
  static Task * CreateTask();
  
  Task();
  virtual ~Task();
  
  /**
   * If the task is not being killed, or if it is held, this will increment the
   * task's reference count and return true. Otherwise, the reference count
   * will not be changed and false will be returned.
   *
   * @critical
   */
  virtual bool Retain();
  
  /**
   * If the task is being killed, this returns false. Otherwise, it increments
   * the reference count of the task and "holds" the task. Now, the task cannot
   * be killed until the task is unheld.
   *
   * @critical
   */
  virtual bool Hold();
  
  /**
   * Unholds the task and then releases it.
   *
   * @critical
   */
  virtual void Unhold();
  
  /**
   * @critical
   */
  virtual void Release();
  
protected:
  /**
   * Called from a kernel thread when the task should be cleaned up.
   * @noncritical
   */
  virtual void Destroy();
  
  IndexSet descriptorIds;
  IndexSet threadIds;
  
  // TODO: address space variable here
  
  // thread list
  uint64_t threadsLock OS_ALIGNED(8);
  Thread * firstThread;
  
  // state information
  uint64_t stateLock OS_ALIGNED(8);
  uint64_t refCount;
  bool isKilled;
  bool isHeld;
  uint64_t killReason;
};

}

#endif
