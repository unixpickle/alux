#ifndef __FIFO_JOB_INFO_HPP__
#define __FIFO_JOB_INFO_HPP__

#include <scheduler/fifo/job-list.hpp>
#include <cstdint>

namespace OS {

namespace Scheduler {

namespace FIFO {

class JobInfo : public UserInfo {
public:
  static JobInfo * ForJob(Job * j);
  
  JobInfo(Job * job);
  JobList::Node node;
  
  uint64_t timerDeadline;
};

}

}

}

#endif
