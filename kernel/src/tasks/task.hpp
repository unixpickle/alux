#ifndef __ALUX_TASK_HPP__
#define __ALUX_TASK_HPP__

#include "../util/identifier.hpp"
#include "../scheduler/garbage-object.hpp"
#include "../containers/thread-list.hpp"
#include <anidmap/id-object>
#include <anarch/api/memory-map>
#include <anarch/assert>

namespace Alux {

class Scheduler;

/**
 * A task is an abstract container of zero or more threads.
 * 
 * A task can be "killed". When a task is killed, it will terminate all of the
 * threads that run under it. A task's lifecycle is managed with its [Retain],
 * [Hold], [Release], and [Unhold] methods. This ensures that a task is not
 * killed while something is using it or while one of its threads is running
 * important kernel code.
 *
 * After initialization, a task is still not ready for use. The task must be
 * added to a scheduler's task list so that it has a unique identifier. Threads
 * may not be added to a task until the task belongs to a task list. You can
 * add a thread to its scheduler's task list using the [AddToScheduler] method.
 *
 * After a task has been killed and fully released and unheld, it will push
 * itself to its scheduler's garbage collector. The garbage collector will call
 * the task's [Dealloc] method. The default [Dealloc] method calls [Dealloc] on
 * each of its threads. It removes the task from the scheduler's task list. The
 * one thing [Dealloc] does not do is **deallocate the instance**. Your
 * subclass will need to do this manually!
 */
class Task : public GarbageObject, public anidmap::IdObject {
public:
  /**
   * Create this task with a given scheduler and user identifier. The created
   * task will not automatically be added to the scheduler.
   */
  Task(Identifier userId, Scheduler & scheduler);
  
  /**
   * Subclasses should be able to do things in their destructors.
   * @noncritical
   */
  virtual ~Task() {
  }
  
  /**
   * The task was terminated by its own volition.
   */
  static const int KillReasonNormal = 0;
  
  /**
   * The task terminated by its own volition due to an error that occured
   * internally.
   */
  static const int KillReasonAbort = 1;
  
  /**
   * The task was terminated by the kernel because it made a request that it
   * was not privileged enough to make.
   */
  static const int KillReasonPermissions = 2;
  
  /**
   * Override this in a subclass to return the task's memory map.
   * @ambicritical
   */
  virtual anarch::MemoryMap & GetMemoryMap() = 0;
  
  /**
   * Override this in a subclass to return `true` if the task is an instance of
   * [UserTask].
   * @ambicritical
   */
  virtual bool IsUserTask() = 0;
  
  /**
   * Add this task to its scheduler's task list. Returns `false` if the task
   * could not be added to the scheduler (i.e. no more PIDs are available).
   * @noncritical
   */
  bool AddToScheduler();
  
  /**
   * If this task has not been killed or it is being held, this will increment
   * the task's retain counter and return `true`. Otherwise, it will return
   * `false`.
   * @ambicritical
   */
  bool Retain();
  
  /**
   * Decrement the task's retain counter. If the task's retain counter and its
   * hold counter reach zero and it has been killed, the task will be pushed to
   * the garbage collector.
   * @ambicritical
   */
  void Release();
  
  /**
   * If the task has not been killed, increment the hold counter and return
   * `true`. Otherwise, return `false`.
   * @ambicritical
   */
  bool Hold();
  
  /**
   * Decrement the task's hold counter. If the task's retain counter and hold
   * counter reach zero and the task has been killed, it will be pushed to the
   * garbage collector.
   * @ambicritical
   */
  void Unhold();
  
  /**
   * "Kill" the task and set its kill reason. In order to call this on a task,
   * you must have the task retained or held, so this call will not deallocate
   * the task right away.
   * @ambicritical
   */
  void Kill(int reason);
  
  /**
   * If [Kill] has been called, this returns the kill reason that was passed as
   * an argument to it. Otherwise, it returns -1.
   * @ambicritical
   */
  int GetKillReason();
  
  /**
   * Get the scheduler to which this task belongs. You should only call this
   * method after the task is added to its scheduler!
   * @ambicritical
   */
  inline Scheduler & GetScheduler() {
    assert(inScheduler);
    return scheduler;
  }
  
  /**
   * Get the task's user identifier.
   * @ambicritical
   */
  inline Identifier GetUserIdentifier() {
    return uid;
  }
  
  /**
   * Get the list of threads belonging to this task. The task should be added
   * to the scheduler before you attempt to call this method.
   * @ambicritical
   */
  inline ThreadList & GetThreadList() {
    assert(inScheduler);
    return threadList;
  }
  
  /**
   * Remove the task from its scheduler's task list. This does not actually
   * delete the task--that is up to the subclass.
   * @noncritical
   */
  virtual void Dealloc();
  
protected:  
  template <class T, int C>
  friend class anidmap::HashMap;
  
  /**
   * The link in the task list.
   */
  ansa::LinkedList<Task>::Link hashMapLink;
  
private:
  Identifier uid;
  Scheduler & scheduler;
  ThreadList threadList;
  bool inScheduler = false;
  
  // [lifeLock] applies to [retainCount], [holdCount], [killReason], and 
  // [killed].
  anarch::CriticalLock lifeLock;
  int retainCount = 0;
  int holdCount = 1;
  int killReason = -1;
};

}

#endif
