#ifndef __SCHEDULER_TASK_HPP__
#define __SCHEDULER_TASK_HPP__

#include <scheduler-specific/task.hpp>
#include <arch-specific/task.hpp>
#include <cstdint>
#include <macros>

namespace OS {

class Thread;
class GarbageThread;
class PIDPool;

class Task : public ArchTask, public SchedTask {
public:
  static Task * New(bool forKernel); // @noncritical, retained result
  
  ~Task(); // @noncritical
  virtual void Delete(); // @noncritical
  
  void AddThread(Thread * th); // @critical
  void RemoveThread(Thread * th); // @critical
  
  bool Retain(); // @critical
  void Release(); // @critical
  bool Hold(); // @critical
  void Unhold(); // @critical
  void Kill(uint64_t status); // @critical

  uint64_t GetPID();

protected:
  Task * garbageNext;
  Task * pidPoolNext, * pidPoolLast;
  friend class GarbageThread;
  friend class PIDPool;
  
  Task(bool forKernel);

private:
  uint64_t pid;
  
  uint64_t threadsLock OS_ALIGNED(8); // @critical
  Thread * firstThread;
  
  uint64_t stateLock OS_ALIGNED(8); // @critical
  uint64_t retainCount = 1;
  uint64_t holdCount;
  uint64_t killStatus;
  bool isKilled;
  
  void Terminate(); // @critical
};

}

#endif
