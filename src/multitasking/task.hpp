#ifndef __MULTITASKING_TASK_HPP__
#define __MULTITASKING_TASK_HPP__

#include <scheduler/job.hpp>

namespace OS {

class TaskGroup;

/**
 * Equivalent of a Mach thread.
 */
class Task : public Scheduler::Job {
public:
  Task(TaskGroup * group);
  
  TaskGroup * GetTaskGroup();
  Scheduler::JobGroup * GetJobGroup();
  
  virtual void Run(); // doesn't do much
  
protected:
  virtual void Cleanup(); // @noncritical
  
  TaskGroup * group;
  
  Task * groupNext;
  Task * groupLast;
  
  friend class TaskGroup;
};

}

#endif
