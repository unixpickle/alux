#ifndef __SCHEDULER_TYPES_HPP__
#define __SCHEDULER_TYPES_HPP__

#include <cstddef>
#include <cstdint>
#include <common>

namespace OS {

namespace Scheduler {

class Scheduler;

class UserInfo {
public:
  virtual ~UserInfo() {}
};

class JobGroup {
public:
  UserInfo * userInfo;
};

class Job {
public:
  UserInfo * userInfo;
  
  virtual void Run(uint64_t timeout) = 0;
  virtual JobGroup * GetJobGroup();
};

class Context {
public:
  UserInfo * userInfo;
  
  static void Stop(); // @critical
  static Context & GetCurrent(); // @critical
  
  virtual Job * GetJob(); // @critical
  virtual void SetJob(Job * job); // @critical
  virtual void Start(); // @critical
  
protected:
  uint64_t jobLock OS_ALIGNED(8); // @critical
  Job * job;
};

class Scheduler {
public:
  /**
   * This will create a scheduler of whatever base class has been determined by
   * this build.
   * @noncritical
   */
  static void InitializeScheduler(Context ** contexts, size_t count);
  
  /**
   * Returns the global initialized scheduler.
   * @ambicritical
   */
  static Scheduler & GetGlobal();
  
  /**
   * Called when a Job is first constructed before it is used, so that you can
   * allocate userInfo for it.
   * @noncritical
   */
  virtual UserInfo * InfoForJob(Job * aJob) = 0;
  
  /**
   * Called when a JobGroup is first constructed before it is used, so that you
   * can allocate userInfo for it.
   * @noncritical
   */
  virtual UserInfo * InfoForJobGroup(JobGroup * aJobGroup) = 0;
  
  /**
   * Create a new timer. If you call this, you do not need to call PushJob(job)
   * and you should *not* call that; the job will automatically be run after
   * the expired time (or a little after if the precision is low).
   * @noncritical
   */
  virtual void SetTimer(uint64_t fireTime, Job * job, bool prec) = 0;
  
  /**
   * Call this periodically from a kernel thread to do the scheduler's
   * deallocating. The reason this is needed is that deallocation may be
   * inappropriate for certain (critical) subroutines.
   * @noncritical
   */
  virtual void CollectGarbage() = 0;
  
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
};

}

}

#endif
