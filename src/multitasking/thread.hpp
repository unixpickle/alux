#ifndef __MULTITASKING_THREAD_HPP__
#define __MULTITASKING_THREAD_HPP__

#include <scheduler/job.hpp>

namespace OS {

class Task;

/**
 * Equivalent of a Mach thread.
 */
class Thread : public Scheduler::Job {
public:
  Thread(Task * group);
  virtual ~Thread() {}
  
  Task * GetTask();
  Scheduler::JobGroup * GetJobGroup();
  
  virtual void Run(); // doesn't do much
  
protected:
  Task * task;
  
  Thread * taskNext;
  Thread * taskLast;
  
  friend class Task;
};

}

#endif
