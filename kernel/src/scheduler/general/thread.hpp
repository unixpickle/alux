#ifndef __SCHEDULER_THREAD_HPP__
#define __SCHEDULER_THREAD_HPP__

#include <scheduler-specific/thread.hpp>
#include <scheduler/internal/garbage-thread.hpp>

namespace OS {

class Task;
class State;
class HoldScope;

class Thread : public SchedThread, public GarbageThread::Garbage {
public:
  // next 4 methods are all @noncritical and *may* return NULL
  static Thread * NewUser(Task * owner, void * call);
  static Thread * NewUser(Task * owner, void * call, void * arg);
  static Thread * NewKernel(Task * owner, void * call);
  static Thread * NewKernel(Task * owner, void * call, void * arg);
  
  static void InitializeCounter();
  static uint64_t GetCounter();
  
  static void Exit() OS_NORETURN; // @critical
  
  virtual ~Thread();
  
  virtual void Delete(); // @noncritical
  Task * GetTask(); // @ambicritical
  State & GetState();
  
  uint64_t GetThreadId();
  
  bool Retain(); // @ambicritical
  void Release(); // @ambicritical
  
  uint64_t timeoutLock OS_ALIGNED(8) = 0; // @noncritical
  bool shouldClearTimeout = false;
  bool isSleeping = false;
  
protected:
  Thread * taskNext = NULL, * taskLast = NULL;
  friend class Task;
  
  bool isHoldingTask = false;
  friend class HoldScope;
  
  virtual void CleanupGarbage();
  
  uint64_t threadId;
  
  Thread(Task * owner, bool kernel, void * func); // @noncritical
  Thread(Task * owner, bool kernel, void * func, void * arg); // @noncritical
  
private:
  Task * task;
  State * state;
  
  uint64_t retainLock OS_ALIGNED(8) = 0;
  uint64_t retainCount = 1;
  bool isKilled = false;
};

}

#endif
