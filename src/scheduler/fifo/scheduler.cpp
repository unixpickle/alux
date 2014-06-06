#include <scheduler/fifo/types.hpp>
#include <utilities/lock.hpp>
#include <utilities/critical.hpp>

namespace OS {

namespace Scheduler {

namespace FIFO {

Scheduler::Scheduler(Context ** _contexts, size_t _count)
  : contexts(_contexts), count(_count) {
  AssertNoncritical();
  jobLists = new JobList[count];
  for (size_t i = 0; i < count; i++) {
    contexts[i]->userInfo = new ContextInfo();
  }
  ContextInfo * info = static_cast<ContextInfo *>(contexts[0]->userInfo);
  info->isActive = true;
}

UserInfo * Scheduler::InfoForJob(Job * aJob) {
  AssertNoncritical();
  return new JobInfo(aJob);
}

UserInfo * Scheduler::InfoForJobGroup(JobGroup * aJobGroup) {
  AssertNoncritical();
  return NULL;
}

void Scheduler::SetTimer(uint64_t fireTime, Job * job, bool prec) {
  AssertNoncritical();
  // TODO: here, do this
}

void Scheduler::CollectGarbage() {
  AssertNoncritical();
  // TODO: collect garbage
}

void Scheduler::PushJob(Job * job) {
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  
  // TODO: here, if the system is being over-utilized, attempt to wake up a
  // new context to use it.
  
  JobInfo * info = static_cast<JobInfo *>(job->userInfo);
  Context * affinity = info->affinity;
  bool shouldStay = true;
  if (!affinity) shouldStay = false;
  else shouldStay = IsContextActive(affinity);
  if (shouldStay) {
    jobLists[affinity->GetIndex()].PushJob(job);
  } else {
    affinity = LeastUtilizedContext();
    info->affinity = affinity;
    jobLists[affinity->GetIndex()].PushJob(job);
  }
}

Job * Scheduler::PopJob() {
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  Context & context = Context::GetCurrent();
  // TODO: maybe get rid of the task
  return jobLists[context.GetIndex()].NextJob();
}

void Scheduler::DeleteJob(Job * job) {
  AssertCritical();
  ScopeCriticalLock scope(&lock);
  for (size_t i = 0; i < count; i++) {
    if (jobLists[i].RemoveJob(job)) break;
  }
  // TODO: here, remove the job from any timers it may be in
}

/** PRIVATE **/

Context * Scheduler::LeastUtilizedContext() {
  uint64_t numerator, denominator;
  Context * currentContext = contexts[0];
  
  contexts[0]->GetTimeInfo(numerator, denominator);
  if (!denominator) denominator = 1;
  
  for (size_t i = 1; i < count; i++) {
    if (!IsContextActive(contexts[i])) break;
    Context * context = contexts[i];
    uint64_t infoNum, infoDenom;
    context->GetTimeInfo(infoNum, infoDenom);
    if (!infoDenom) infoDenom = 1;
    uint64_t leftNum = numerator * infoDenom;
    uint64_t rightNum = denominator * infoNum;
    if (rightNum < leftNum) currentContext = context;
  }
  
  return currentContext;
}

bool & Scheduler::IsContextActive(Context * context) {
  ContextInfo * info = static_cast<ContextInfo *>(context->userInfo);
  return info->isActive;
}

}

}

}
