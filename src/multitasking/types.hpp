#ifndef __MULTITASKING_TYPES_HPP__
#define __MULTITASKING_TYPES_HPP__

#include <scheduler/types.hpp>

namespace OS {

class Task;

/**
 * Equivalent of a Mach task.
 */
class TaskGroup : public Scheduler::JobGroup {
public:
  virtual void AddTask(Task * task); // @critical
  virtual void RemoveTask(Task * task); // @critical
  
  void Retain(); // @critical
  void Release(); // @critical
  void Hold(); // @critical
  void Kill(); // @critical

protected:
  virtual void Cleanup();
  
  uint64_t tasksLock OS_ALIGNED(8); // @critical
  Task * firstTask;
  
  uint64_t lifeLock OS_ALIGNED(8);
  uint64_t retainCount;
  bool isHeld;
  bool isKilled;
};

/**
 * Equivalent of a Mach thread.
 */
class Task : public Scheduler::Job {
public:
  Task(TaskGroup * group);
  // some state will go here, etc.

  TaskGroup * GetTaskGroup();
  virtual Scheduler::JobGroup * GetJobGroup();
protected:
  TaskGroup * group;
  virtual void Cleanup();
};

}

#endif
