#ifndef __ALUX_TASK_HPP__
#define __ALUX_TASK_HPP__

#include "thread.hpp"
#include "../memory/garbage-object.hpp"
#include <anarch/lock>
#include <anarch/api/user-map>
#include <ansa/nocopy>
#include <ansa/linked-list>

namespace OS {

class Scheduler;
class TaskIdPool;
class IncrementalIdPool;

typedef NativeInt TaskId;

/**
 * An abstract "task" which runs threads.
 */
class Task : protected GarbageObject, public ansa::NoCopy {
public:
  /**
   * Zero-out the object instance counter.
   * @ambicritical
   */
  static void InitializeCounter();
  
  /**
   * Read the object instance counter. The counter is atomic and increases with
   * the number of active Task instances.
   * @ambicritical
   */
  static uint64_t ReadCounter();
  
  /**
   * Every task has an address space, even if that address space is the global
   * one. This returns said address space.
   * @ambicritical
   */
  virtual anarch::MemoryMap & GetMemoryMap() const = 0;
  
  /**
   * Returns `true` if this task can be safely casted to an instance of
   * [UserTask]. Otherwise, this is an instance of [KernelTask].
   */
  virtual bool IsUserTask() const = 0; // @ambicritical
  
  /**
   * Increment the retain counter for this task.
   *
   * If the task has been killed and its hold count is 0, this will return
   * `false`.
   * You should only call this on a task that you know is being held or
   * retained by something else, or if you are holding a lock that the task
   * must hold to destroy itself.
   * 
   * @ambicritical
   */
  bool Retain();
  
  /**
   * Decrement the retain counter for this task.
   *
   * If the retain count and hold count reach zero and the task has been
   * killed, it will be pushed to the garbage thread.
   *
   * @ambicritical
   */
  void Release();
  
  /**
   * Increment the hold counter for this task.
   *
   * If the task has been killed, this returns `false`. Otherwise, it returns
   * `true`.
   *
   * @ambicritical
   */
  bool Hold();
  
  /**
   * Decrement the hold counter for this task. See the description for
   * [Release] to know what may happen when this reaches zero.
   * @ambicritical
   */
  void Unhold();
  
  /**
   * Request that this task be killed. You MUST be holding a reference to this
   * task when you call this (either you held it, or you retained it).
   *
   * You provide a [killStatus] to indicate why the OS decided to kill the
   * task.
   *
   * @ambicritical
   */
  void Kill(uint16_t killStatus); // @ambicritical
  
  /**
   * Return the scheduler this task was created on.
   * @ambicritical
   */
  Scheduler & GetScheduler() const;
  
  /**
   * Return the PID of this task.
   * @ambicritical
   */
  TaskId GetId() const;
  
protected:
  /**
   * Create a new task with a hold count of 1 and a retain count of 0. The
   * returned task will not yet have a PID, so you should call Init() on it
   * immediately to allocate one.
   * @noncritical
   */
  Task(Scheduler &);
  
  /**
   * Decrements the object instance counter.
   * @ambicritical
   */
  virtual ~Task();
  
  /**
   * Allocates a PID for this task and does other setup that requires a full
   * v-table.
   * @noncritical
   */
  virtual void Init();
  
  /**
   * Remove this task from the task ID list. Removes every thread from the
   * scheduler if it has been scheduled.
   * @noncritical
   */
  virtual void Deinit();
  
  friend class Thread;
  friend class IncrementalIdPool;
  
  /**
   * The link for the TaskIdPool pool.
   */
  ansa::LinkedList<Task>::Link pidPoolLink;
  
  /**
   * Add a thread to this task and allocate an ID for it.
   * @noncritical
   */
  ThreadId AddThread(Thread &);
  
  /**
   * Remove a thread from this task and "free" its ID.
   * @noncritical
   */
  void RemoveThread(Thread &);
  
  /**
   * Find a thread given its identifier. Returns NULL if no thread is found.
   * @noncritical
   */
  Thread * LookupThread(ThreadId);
  
private:
  Scheduler & scheduler;
  TaskId identifier = 0;
  
  anarch::CriticalLock stateLock;
  int retainCount = 0;
  int holdCount = 1;
  uint16_t killStatus;
  bool isKilled = false;
  
  anarch::NoncriticalLock threadsLock;
  ansa::LinkedList<Thread> threads;
  ThreadId threadCounter = 0; // TODO: use a real pool for this
};

}

#endif
