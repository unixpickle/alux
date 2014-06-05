#ifndef __SCHEDULER_JOB_HPP__
#define __SCHEDULER_JOB_HPP__

namespace OS {

class Scheduler;

class Job {
public:
  virtual void Run(uint64_t timeout) = 0;
  
protected:
  void * userInfo; // for the Scheduler
  friend class Scheduler;
};

}

#endif