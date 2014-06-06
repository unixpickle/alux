#include <scheduler/fifo/types.hpp>

namespace OS {

namespace Scheduler {

namespace FIFO {

Scheduler::Scheduler(Context ** _contexts, size_t _count)
  : contexts(_contexts), count(_count) {
  jobLists = new JobList[count];
  for (size_t i = 0; i < count; i++) {
    contexts[i]->userInfo = new ContextInfo();
  }
}

UserInfo * Scheduler::InfoForJob(Job * aJob) {
  return new JobInfo(aJob);
}

UserInfo * Scheduler::InfoForJobGroup(JobGroup * aJobGroup) {
  return NULL;
}

void Scheduler::SetTimer(uint64_t fireTime, Job * job, bool prec) {
  // TODO: here, do this
}

void Scheduler::CollectGarbage() {
  // TODO: collect garbage
}

void Scheduler::PushJob(Job * job) {
  
}

Job * Scheduler::PopJob() {
  return NULL;
}

void Scheduler::DeleteJob(Job * job) {
  
}

}

}

}
