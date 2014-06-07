#ifndef __MULTITASKING_TASK_GROUP_HPP__
#define __MULTITASKING_TASK_GROUP_HPP__

#include <scheduler/job-group.hpp>
#include <cstdint>
#include <common>

namespace OS {

class Task;

/**
 * Equivalent of a Mach task.
 */
class TaskGroup : public Scheduler::JobGroup {
public:
  virtual void AddTask(Task * task); // @critical
  virtual void RemoveTask(Task * task); // @critical
  virtual ~TaskGroup() {} // @noncritical
  
  bool Retain(); // @critical
  void Release(); // @critical
  bool Hold(); // @critical
  void Unhold(); // @critical
  void Kill(); // @critical

protected:
  virtual void Cleanup(); // @noncritical
  
  uint64_t tasksLock OS_ALIGNED(8); // @critical
  Task * firstTask;
  
  uint64_t lifeLock OS_ALIGNED(8);
  uint64_t retainCount;
  uint64_t holdCount;
  bool isKilled;
};

}

#endif
