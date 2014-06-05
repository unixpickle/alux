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
  /**
   * @noncritical
   */
  virtual ~Task();
  
  /**
   * Returns true unless the task has been killed and is not being held.
   * @critical
   */
  virtual bool Retain();
  
  /**
   * Returns true unless the task has been killed.
   * @critical
   */
  virtual bool Hold();
  
  /**
   * @critical
   */
  virtual void Unhold();
  
  /**
   * @critical
   */
  virtual void Release();
  
  /**
   * Request that the task be killed. If the task has already been killed, that
   * kill reason will not be changed.
   * @critical
   */
  virtual void Kill(uint64_t reason);
  
protected:
  /**
   * Perform last-minute cleanup for the task.
   * @noncritical
   */
  virtual void Destroy();
  
  // state information (from hell)
  uint64_t stateLock OS_ALIGNED(8); // @critical
  uint64_t refCount;
  bool isKilled;
  bool isHeld;
  uint64_t killReason;
};

}

#endif
