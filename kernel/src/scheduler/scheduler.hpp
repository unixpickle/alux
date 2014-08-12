#ifndef __ALUX_SCHEDULER_HPP__
#define __ALUX_SCHEDULER_HPP__

#include "task-id-pool.hpp"
#include <ansa/lock>

namespace OS {

class GarbageCollector;

/**
 * An abstract "scheduler" which maintains a list of threads that it may
 * schedule. A thread will not be scheduled when it cannot be retained.
 */
class Scheduler {
public:
  /**
   * Add a new thread to the scheduler.
   * @noncritical
   */
  virtual void Add(Thread & th) = 0;
  
  /**
   * Remove a thread from the scheduler. The thread may not have been added to
   * any scheduler, in which case this method should do nothing.
   * @noncritical
   */
  virtual void Remove(Thread & th) = 0;
  
  /**
   * Sleep the current thread until the elapsed time has been completed or the
   * thread's timeout is cleared. After the timeout has been set, a lock is
   * unlocked.
   * @ambicritical
   */
  virtual void SetTimeout(uint64_t deadline, ansa::Lock & unlock) = 0;
  
  /**
   * Set an infinite timeout and the unlock a lock.
   *
   * See note on ClearTimeout().
   *
   * @ambicritical
   */
  virtual void SetInfiniteTimeout(ansa::Lock & unlock) = 0;
  
  /**
   * Clear the timeout for a given thread.
   *
   * The garbage thread may call this at any time, so it should not attempt to
   * seize any locks or Release() any objects to avoid deadlock.
   *
   * @ambicritical
   */
  virtual void ClearTimeout(Thread & thread) = 0;
  
  /**
   * Resign control from the current thread and yield to the scheduler.
   * @ambicritical
   */
  virtual void Yield() = 0;
  
  /**
   * Return the garbage collector in order to create new garbage objects.
   */
  virtual GarbageCollector & GetGarbageCollector() const = 0;
  
  /**
   * Return the PID pool to use for tasks in this scheduler.
   */
  virtual TaskIdPool & GetTaskIdPool() const = 0;
};

}

#endif
