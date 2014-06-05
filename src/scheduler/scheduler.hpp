#ifndef __SCHEDULER_SCHEDULER_HPP__
#define __SCHEDULER_SCHEDULER_HPP__

#include <cstddef>
#include <cstdint>

namespace OS {

class Job;
class Context;
class Timer;

class Scheduler {
public:
  /**
   * Create a scheduler.
   * @param contexts The execution contexts, ordered by priority (that is, you
   * would rather the first run than the second, etc.)
   * @param count The total number of contexts in the array.
   * @noncritical
   */
  Scheduler(Context ** contexts, size_t count);
  
  /**
   * Called when a Job is first constructed before it is used, so that you can
   * allocate userInfo and stuff.
   * @noncritical
   */
  virtual void JobConstructed(Job * aJob) = 0;
  
  /**
   * Called right before a Job is completely destroyed so that you may free the
   * user info and stuff.
   * @noncritical
   */
  virtual void JobDestroyed(Job * aJob) = 0;
  
  /**
   * Create a new timer. If you call this, you do not need to call PushJob(job)
   * and you should *not* call that; the job will automatically be run after the
   * expired time (or a little after if the precision is low).
   * @noncritical
   */
  virtual void SetTimer(uint64_t fireTime, Job * job, bool prec) = 0;
  
  /**
   * Pushes a job to the scheduler so it may be executed
   * @critical
   */
  virtual void PushJob(Job * job) = 0;
  
  /**
   * Returns the next job to attempt to execute
   * @critical
   */
  virtual Job * PopJob() = 0;
  
  /**
   * @critical
   */
  virtual void DeleteJob(Job * job) = 0;
  
protected:
  Context ** contexts;
  size_t contextCount;
};

}

#endif
