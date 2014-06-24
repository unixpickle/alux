#ifndef __SCHEDULER_TASK_HPP__
#define __SCHEDULER_TASK_HPP__

#include <scheduler-specific/task.hpp>
#include <utilities/index-set.hpp>
#include <cstdint>
#include <macros>

namespace OS {

class Thread;
class GarbageThread;
class PIDPool;
class UserMap;
class AddressSpace;

class Task : public SchedTask {
public:
  static Task * New(bool forKernel);
  
  ~Task(); // @noncritical
  virtual void Delete(); // @noncritical
  virtual bool IsKernel();
  
  void AddThread(Thread * th); // @noncritical
  void RemoveThread(Thread * th); // @noncritical
  
  bool Retain(); // @critical
  void Release(); // @critical
  bool Hold(); // @critical
  void Unhold(); // @critical
  void Kill(uint64_t status); // @critical

  uint64_t GetPID();
  AddressSpace & GetAddressSpace();
  UserMap * GetUserAddressSpace();

protected:
  Task * garbageNext;
  Task * pidPoolNext, * pidPoolLast;
  friend class GarbageThread;
  friend class PIDPool;
  
  Task(bool forKernel);

private:
  uint64_t pid;
  bool isKernel;
  
  uint64_t threadsLock OS_ALIGNED(8); // @critical
  Thread * firstThread;
  
  IndexSet<0x10> threadIds;
  
  uint64_t stateLock OS_ALIGNED(8); // @critical
  uint64_t retainCount = 1;
  uint64_t holdCount;
  uint64_t killStatus;
  bool isKilled;
  
  UserMap * userSpace;
  
  void Terminate(); // @critical
};

}

#endif
