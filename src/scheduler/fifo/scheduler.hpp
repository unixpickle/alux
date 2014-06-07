#ifndef __FIFO_SCHEDULER_HPP__
#define __FIFO_SCHEDULER_HPP__

#include <scheduler/fifo/job-list.hpp>
#include <scheduler/scheduler.hpp>
#include <cstdint>
#include <common>

namespace OS {

namespace Scheduler {

namespace FIFO {

class Scheduler : public OS::Scheduler::Scheduler {
public:
  static const int Jiffy = 6000; // 6000 times a minute (100 Hz)
  
  Scheduler();

  virtual UserInfo * InfoForJob(Job * aJob);
  virtual UserInfo * InfoForJobGroup(JobGroup * aJobGroup);
  virtual void SetTimer(uint64_t fireTime, bool prec);
  virtual void SetInfiniteTimer();
  virtual bool UnsetTimer(Job * job);
  virtual void Tick();
  virtual void AddJob(Job * job);
  virtual void DeleteJob(Job * job);

protected:
  uint64_t lock OS_ALIGNED(8); // @critical
  JobList jobList;
};

}

}

}

#endif
