#ifndef __SCHEDULER_CONTEXT_HPP__
#define __SCHEDULER_CONTEXT_HPP__

#include <common>
#include <cstdint>

namespace OS {

class Job;
class Scheduler;

class Context {
public:
  /**
   * Sleep the current context until a WakeUp() on it is called.
   * @critical
   */
  static void Sleep();
  
  /**
   * Return the current context. Needless to say, this is architecture
   * independent.
   * @critical
   */
  static Context * GetContext();
  
  /**
   * @critical
   */
  virtual Job * GetJob();
  
  /**
   * @critical
   */
  virtual void SetJob(Job * job);
  
  /**
   * @critical
   */
  virtual void WakeUp();
  
protected:
  uint64_t jobLock OS_ALIGNED(8); // @critical
  Job * currentJob;
  
  void * userInfo; // for the Scheduler
  
  friend class Scheduler;
};

}

#endif
