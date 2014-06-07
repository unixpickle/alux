#include <scheduler/fifo/types.hpp>
#include <arch/general/clock.hpp>
#include <arch/general/cpu.hpp>
#include <utilities/lock.hpp>
#include <utilities/critical.hpp>

namespace OS {

namespace Scheduler {

namespace FIFO {

Scheduler::Scheduler() {
  AssertNoncritical();
}

UserInfo * Scheduler::InfoForJob(Job * aJob) {
  AssertNoncritical();
  return new JobInfo(aJob);
}

UserInfo * Scheduler::InfoForJobGroup(JobGroup *) {
  AssertNoncritical();
  return NULL;
}

void Scheduler::SetTimer(uint64_t fireTime, bool) {
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  
  CPU & cpu = CPU::GetCurrent();
  Job * job = cpu.GetJob();
  assert(job != NULL);
  JobInfo::ForJob(job)->timerDeadline = fireTime;
}

bool Scheduler::UnsetTimer(Job * job) {
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  JobInfo * info = JobInfo::ForJob(job);
  bool ret = info->timerDeadline != 0;
  info->timerDeadline = 0;
  return ret;
}

void Scheduler::Tick() {
  AssertCritical();
  Job * job;
  Clock & clock = Clock::GetGlobal();
  uint64_t now = clock.GetTime();
  uint64_t nextTick = now + clock.GetTicksPerMin() / Jiffy;
  {
    ScopeCriticalLock scope(&lock);
    job = jobList.NextJob();
    while (job) {
      JobInfo * info = JobInfo::ForJob(job);
      if (info->timerDeadline > now) {
        if (info->timerDeadline < nextTick) {
          nextTick = info->timerDeadline;
        }
        jobList.PushJob(job);
        job = jobList.NextJob();
        continue;
      }
      info->timerDeadline = 0;
      break;
    }
  }
  
  CPU::SetTimeout(nextTick - now);
  if (job) {
    job->Run();
  } else {
    CPU::WaitUntilTick();
  }
}

void Scheduler::AddJob(Job * job) {
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  jobList.PushJob(job);
}

void Scheduler::DeleteJob(Job * job) {
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  jobList.RemoveJob(job);
}

}

}

}
