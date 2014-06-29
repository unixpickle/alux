#ifndef __SCHEDULER_TASK_HPP__
#define __SCHEDULER_TASK_HPP__

#include <scheduler-specific/task.hpp>
#include <scheduler/internal/garbage-thread.hpp>
#include <utilities/index-set.hpp>
#include <cstdint>
#include <macros>

namespace OS {

class Thread;
class PIDPool;
class UserMap;
class AddressSpace;

class Task : public SchedTask, public GarbageThread::Garbage {
public:
  static Task * New(bool forKernel); // @noncritical
  
  static void InitializeCounter();
  static uint64_t GetCounter();
  
  static void Exit(uint64_t status) OS_NORETURN; // @critical
  
  ~Task(); // @noncritical
  virtual void Delete(); // @noncritical
  virtual bool IsKernel();
  
  void AddThread(Thread * th); // @noncritical
  void RemoveThread(Thread * th); // @noncritical
  void UnsleepThreadById(uint64_t ident); // @noncritical
  
  bool Retain(); // @critical
  void Release(); // @critical
  bool Hold(); // @critical
  void Unhold(); // @critical
  void Kill(uint64_t status); // @critical

  uint64_t GetPID();
  AddressSpace & GetAddressSpace();
  UserMap * GetUserAddressSpace();

protected:
  Task * pidPoolNext, * pidPoolLast;
  friend class PIDPool;
  
  Task(bool forKernel);
  virtual void CleanupGarbage();

private:
  uint64_t pid;
  bool isKernel;
  
  uint64_t threadsLock OS_ALIGNED(8); // @noncritical
  Thread * firstThread;
  
  IndexSet<0x10> threadIds;
  IndexSet<0x20> descriptors;
  
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
