#ifndef __ALUX_THREAD_HPP__
#define __ALUX_THREAD_HPP__

#include "../memory/garbage-object.hpp"
#include <anarch/api/state>
#include <anarch/types>
#include <anarch/lock>
#include <ansa/nocopy>

namespace OS {

class Task;
class Scheduler;

typedef NativeInt ThreadId;

/**
 * A single thread of execution. Threads are scheduled by a scheduler and
 * belong to a task.
 */
class Thread : public ansa::NoCopy, protected GarbageObject {
public:
  /**
   * Zero-out the instance counter.
   * @ambicritical
   */
  static void InitializeCounter();
  
  /**
   * Read the instance counter.
   * @ambicritical
   */
  static uint64_t ReadCounter();
  
  /**
   * Allocate a new thread, initialize it by adding it to a task, and then
   * return it. The thread will not be scheduled automatically.
   * @noncritical
   */
  static Thread & New(Task &, anarch::State &);
  
  /**
   * Get the task to which this thread belongs.
   * @ambicritical
   */
  Task & GetTask() const;
  
  /**
   * Get the state to which this thread belongs.
   * @ambicritical
   */
  anarch::State & GetState() const;
  
  /**
   * Increment the retain count on this thread.
   *
   * This will attempt to retain the thread's owning task. If the owning task
   * cannot be retained, or this thread has been killed, this will return
   * `false` and the retain count will not be changed.
   * 
   * @ambicritical
   */
  bool Retain();
  
  /**
   * Decrement the retain count and destroy this thread if its retain count has
   * reached zero.
   * @ambicritical
   */
  void Release();
  
  /**
   * Get this thread's identifier.
   * @ambicritical
   */
  ThreadId GetId() const;
  
protected:
  Thread(Task & task, anarch::State & state); // @noncritical
  ~Thread(); // @noncritical
  
  bool isScheduled = false;
  
  friend class Task;  
  ansa::LinkedList<Thread>::Link taskLink;
  
  friend class Scheduler;
  ansa::LinkedList<Thread>::Link schedulerLink;
  void * schedulerUserInfo;
  
  virtual void Destroy(); // @noncritical
  
private:
  Task & task;
  anarch::State & state;
  
  anarch::CriticalLock retainLock;
  int retainCount = 1;
  bool isKilled = false;
  
  ThreadId identifier;
};

}

#endif
