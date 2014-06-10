#ifndef __SCHEDULER_TASK_HPP__
#define __SCHEDULER_TASK_HPP__

#include <scheduler-specific/task.hpp>
#include <arch-specific/task.hpp>
#include <cstdint>
#include <common>

namespace OS {

class Scheduler;
class Thread;

class Task : public ArchTask, public SchedTask {
public:
  static Task * New(bool forKernel); // @noncritical
  
  ~Task(); // @noncritical
  void Delete(); // @noncritical
  
  void AddThread(Thread * th); // @critical
  void RemoveThread(Thread * th); // @critical
  
  bool Retain(); // @critical
  void Release(); // @critical
  bool Hold(); // @critical
  void Unhold(); // @critical
  void Kill(uint64_t status); // @critical

private:
  Task(bool forKernel);
  
  uint64_t threadsLock OS_ALIGNED(8); // @critical
  Thread * firstThread;
  
  uint64_t stateLock OS_ALIGNED(8); // @critical
  uint64_t retainCount;
  uint64_t holdCount;
  uint64_t killStatus;
  bool isKilled;
  
  void Terminate(); // @critical
};

}

#endif
