#ifndef __FIFO_SCHEDULER_HPP__
#define __FIFO_SCHEDULER_HPP__

#include <scheduler/fifo/job-list.hpp>
#include <scheduler/fifo/timer.hpp>
#include <scheduler/scheduler.hpp>
#include <common>

namespace OS {

namespace FIFO {

class Scheduler : public OS::Scheduler {
public:
  Scheduler(Context ** contexts, size_t count);
  
  virtual void JobConstructed(Job * aJob);
  virtual void JobDestroyed(Job * aJob);
  virtual void SetTimer(uint64_t fireTime, Job * job, bool prec);
  virtual void PushJob(Job * job);
  virtual Job * PopJob();
  virtual void DeleteJob(Job * job);
  
protected:
  uint64_t jobLock OS_ALIGNED(8); // @critical
  JobList * jobLists;
};

}

}

#endif
