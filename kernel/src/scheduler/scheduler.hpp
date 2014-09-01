#ifndef __ALUX_SCHEDULER__
#define __ALUX_SCHEDULER__

#include "garbage-collector.hpp"
#include "../identifiers/pool-id-map.hpp"
#include "../tasks/task.hpp"
#include <anarch/stdint>
#include <ansa/lock>

namespace Alux {

class Scheduler {
public:
  typedef PoolIdMap<Task, 0x100> TaskMap;
  
  Scheduler(); // @noncritical
  
  virtual ~Scheduler(); // @noncritical
  
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
   * Return the garbage collector for this scheduler.
   * @ambicritical
   */
  virtual GarbageCollector & GetGarbageCollector() = 0;
  
  TaskMap & GetTaskIdMap();
  
protected:
  friend class GarbageCollector;
  
  /**
   * Set an infinite timeout on the garbage thread.
   *
   * This may be called from the garbage collector's internals. Thus, nothing
   * should be retained or released in this method until [unlock] is unlocked
   * to prevent deadlock.
   *
   * @ambicritical
   */
  virtual void SetGarbageTimeout(ansa::Lock & unlock) = 0;
  
  /**
   * Clear the timeout on the garbage thread.
   *
   * This may be called any time you release or unhold a [GarbageObject]. Thus,
   * your scheduler should take care not to seize any locks in here that it
   * might be holding while it releases or retains a thread.
   *
   * @ambicritical
   */
  virtual void ClearGarbageTimeout() = 0;
  
  inline static void *& ThreadUserInfo(Thread & th) {
    return th.schedulerUserInfo;
  }
  
private:
  TaskMap taskIdMap;
};

}

#endif
