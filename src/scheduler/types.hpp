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
  
  virtual void Run() = 0; // @critical (no return)
  virtual JobGroup * GetJobGroup() = 0; // @ambicritical
};

class Context {
public:
  UserInfo * userInfo;
  
  static void Halt(); // @critical
  static void WaitTimeout(); // @critical
  static void SetTimeout(uint64_t timeout); // @critical
  static Context & GetCurrent(); // @critical
  
  virtual Job * GetJob(); // @critical
  virtual void SetJob(Job * job); // @critical
  virtual void Unhalt(); // @critical
  virtual size_t GetIndex(); // @ambicritical
  
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
   * Set a timer for the current job. After you call this, you should call
   * Tick() from the same critical section.
   * @critical
   */
  virtual void SetTimer(uint64_t fireTime, bool prec) = 0;
  
  /**
   * Unset a timer for some job. If a timer was set for the job, this will
   * rescheduler the job regularly.
   * @return true if a timer was removed; false otherwise
   * @critical
   */
  virtual bool UnsetTimer(Job * job) = 0;
  
  /**
   * Called when the current timeout fires.
   * @critical
   */
  virtual void Tick() = 0;
  
  /**
   * @critical
   */
  virtual void AddJob(Job * job) = 0;
  
  /**
   * @critical
   */
  virtual void DeleteJob(Job * job) = 0;
};

}

}

#endif
