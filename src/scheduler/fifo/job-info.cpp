#include <scheduler/fifo/types.hpp>

namespace OS {

namespace Scheduler {

namespace FIFO {

JobInfo * JobInfo::ForJob(Job * j) {
  return static_cast<JobInfo *>(j->userInfo);
}

JobInfo::JobInfo(Job * job) : node(job) {}

}

}

}
