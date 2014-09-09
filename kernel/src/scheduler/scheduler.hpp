#ifndef __ALUX_SCHEDULER__
#define __ALUX_SCHEDULER__

#include "garbage-collector.hpp"
#include "../containers/task-list.hpp"
#include <anarch/stdint>
#include <ansa/lock>

namespace Alux {

class Scheduler {
public:  
  /**
   * @noncritical
   */
  virtual ~Scheduler() {
  }
  
  /**
   * Add a thread to the scheduler.
   * @noncritical
   */
  virtual void Add(Thread &) = 0;
  
  /**
   * Remove a thread from the scheduler.
   * @noncritical
   */
  virtual void Remove(Thread &) = 0;
  
  /**
   * Unschedule the current thread until a time on the system clock.
   * @ambicritical
   */
  virtual void SetTimeout(uint64_t deadline) = 0;
  
  /**
   * Like the other [SetTimeout], but also releases a lock once the timeout has
   * been set. This way, you can manage to perform atomic timeout operations.
   * @ambicritical
   */
  virtual void SetTimeout(uint64_t deadline, ansa::Lock & unlock) = 0;
  
  /**
   * Set a timeout that will never end unless it is cleared by some other
   * thread.
   * @ambicritical
   */
  virtual void SetInfiniteTimeout() = 0;
  
  /**
   * Like the other [SetInfiniteTimeout], but releases a lock after the timeout
   * has been set.
   * @ambicritical
   */
  virtual void SetInfiniteTimeout(ansa::Lock & unlock) = 0;
  
  /**
   * Clear the timeout for a given thread. If the thread is in the middle of a
   * call to [SetTimeout] or [SetInfiniteTimeout], this may or may not clear
   * the newly created timeout. To deal with this, you should seize a lock
   * before calling [ClearTimeout], [SetTimeout], or [SetInfiniteTimeout] that
   * you unlock using the atomic unlock mechanism provided by the latter two.
   * @ambicritical
   */
  virtual void ClearTimeout(Thread &) = 0;
  
  /**
   * Manually tell the scheduler to move on to other tasks early. For some
   * schedulers, a call to yield might actually lead straight back to the same
   * thread being run if it is of higher priority, etc.
   * @ambicritical
   */
  virtual void Yield() = 0;
  
  /**
   * Call this ONCE to start the scheduler. The scheduler is responsible for
   * waking up other CPUs and performing context switches from here on out.
   * @ambicritical
   */
  virtual void Run() = 0;
  
  /**
   * Get the garbage collector for this scheduler.
   * @ambicritical
   */
  virtual GarbageCollector & GetGarbageCollector() = 0;
  
  /**
   * Get the task list. While you can technically get the task list from a
   * critical section, you cannot manipulate or search it while in a critical
   * section.
   * @ambicritical
   */
  inline TaskList & GetTaskList() {
    return taskList;
  }
  
protected:
  friend class GarbageCollector;
  
  /**
   * Clear the timeout on the garbage thread.
   * @ambicritical
   */
  virtual void ClearGarbageTimeout() = 0;
  
  /**
   * Friend functions are not inherited (thanks a lot, C++ standards 
   * committee.) Instead, I have to make a method in this base class so
   * schedulers can access a thread's protected `schedulerUserInfo` field.
   */
  inline static void *& ThreadUserInfo(Thread & th) {
    return th.schedulerUserInfo;
  }
  
private:
  TaskList taskList;
};

}

#endif
