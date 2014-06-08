#ifndef __BASE_SCHEDULER_HPP__
#define __BASE_SCHEDULER_HPP__

#include <scheduler/base/thread.hpp>

namespace OS {

namespace BaseScheduler {

class Scheduler {
public:
  // operations acting on the scheduler as a whole
  void AddThread(Thread *) {} // @critical
  void RemoveThread(Thread *) {} // @critical
  
  // operations acting on the current thread or a specific thread
  void SetTimeout(uint64_t deadline, bool precise) {} // @critical
  void SetInfiniteTimeout() {} // @critical
  bool ClearTimeout(Thread *) { return false; } // @critical
  void Resign() {} // @critical
  
  // called by the architecture when a CPU timer fires or when the scheduler
  // requests a blank slate
  void Tick() {} // @critical
};

}

}

#endif
