#ifndef __MULTITASKING_TASK_HPP__
#define __MULTITASKING_TASK_HPP__

#include <scheduler/job-group.hpp>
#include <cstdint>
#include <common>

namespace OS {

class Thread;

/**
 * Equivalent of a Mach task.
 */
class Task : public Scheduler::JobGroup {
public:
  virtual void AddThread(Thread * th); // @critical
  virtual void RemoveThread(Thread * th); // @critical
  virtual ~Task(); // @noncritical
  
  bool Retain(); // @critical
  void Release(); // @critical
  bool Hold(); // @critical
  void Unhold(); // @critical
  void Kill(); // @critical

protected:
  uint64_t threadsLock OS_ALIGNED(8); // @critical
  Thread * firstThread;
  
  Task * garbageNext;
  Task * garbageLast;
  
  uint64_t lifeLock OS_ALIGNED(8);
  uint64_t retainCount;
  uint64_t holdCount;
  bool isKilled;
};

}

#endif
