#ifndef __ALUX_THREAD_HPP__
#define __ALUX_THREAD_HPP__

#include "sleep-state.hpp"
#include "poll-state.hpp"
#include "../scheduler/garbage-object.hpp"
#include "../containers/thread-port-list.hpp"
#include <anarch/api/state>
#include <anarch/lock>
#include <ansa/linked-list>

namespace Alux {

class Task;
class HoldScope;

/**
 * A thread is a single flow of execution. A thread can be slept, resumed,
 * and terminated.
 *
 * When a thread is initialized, it is not yet ready for action. A thread must
 * first be added to its task's thread list. Then, the thread must be added to
 * the task's scheduler.
 *
 * Threads use a [Retain]/[Release] mechanism to ensure that a thread is not
 * deallocated while it is running. The thread's [Retain] method also retains
 * its task, ensuring that a task cannot die while one of its threads is
 * running.
 *
 * Threads may not kill other threads. The only exception is when the thread's
 * task is terminated.
 */
class Thread : public GarbageObject, public anidmap::IdObject {
public:
  /**
   * Destroy the state of this thread.
   */
  ~Thread();
  
  /**
   * Allocate and initialize a [Thread]. If the allocation fails, the kernel
   * will Panic().
   * @noncritical
   */
  static Thread & New(Task &, anarch::State &);
  
  /**
   * Get the current thread by reading this hardware thread's user-info.
   * @ambicritical
   */
  static Thread * GetCurrent();
  
  /**
   * Set the current thread for this CPU.
   * @critical
   */
  static void SetCurrent(Thread *);
  
  /**
   * Exit the current thread.
   * @ambicritical
   */
  static void ExitCurrent() ANSA_NORETURN;
  
  /**
   * Initialize a thread. The thread will not be added to its [task], nor will
   * it be scheduled. The thread will be retained. Thus, this consumes the
   * reference that the caller had to [task].
   * @ambicritical
   */
  Thread(Task & task, anarch::State &);
  
  /**
   * Add this thread to its owning task. If the task is out of thread IDs, this
   * fails and returns `false`.
   * @noncritical
   */
  bool AddToTask();
  
  /**
   * Add this thread to the scheduler. You should only call this once the
   * thread has been added to its owning task.
   * @noncritical
   */
  void AddToScheduler();
  
  /**
   * Get the owning task. You should not call this until the thread has been
   * added to its task.
   * @ambicritical
   */
  inline Task & GetTask() {
    assert(inTask);
    return task;
  }
  
  /**
   * Get the state underlying this thread.
   * @ambicritical
   */
  inline anarch::State & GetState() {
    return state;
  }
  
  /**
   * Increment the task's retain count and the thread's retain count and return
   * `true`. If the thread has been killed or the task cannot be retained,
   * `false` will be returned.
   * @ambicritical
   */
  bool Retain();
  
  /**
   * Decrement this thread's retain count. If the retain count reaches zero and
   * the thread has been killed, the thread will be pushed to the garbage
   * collector. Otherwise, the thread's owning task will be released.
   * @ambicritical
   */
  void Release();
  
  /**
   * If this thread was killed, remove it from its owning task and from the
   * scheduler.
   * @noncritical
   */
  virtual void Dealloc();
  
protected:
  template <class T, int C>
  friend class anidmap::HashMap;
  ansa::LinkedList<Thread>::Link hashMapLink;
  
  friend class Scheduler;
  void * schedulerUserInfo = NULL;
  
  friend class HoldScope;
  bool holdingTask = false;
  void Kill(); // @ambicritical
  
  friend class SleepState;
  SleepState sleepState;
  
  friend class PollState;
  PollState pollState;
  
  friend class ThreadPort;
  
  inline ThreadPortList & GetPortList() {
    return portList;
  }
  
private:
  Task & task;
  anarch::State & state;
  
  ThreadPortList portList;
    
  // [lifeLock] controls both [retainCount] and [killed].
  anarch::CriticalLock lifeLock;
  int retainCount = 1;
  bool killed = false;
  
  bool inTask = false;
  bool inScheduler = false;
};

}

#endif
