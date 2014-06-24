#ifndef __SCHEDULER_THREAD_HPP__
#define __SCHEDULER_THREAD_HPP__

#include <scheduler-specific/thread.hpp>
#include <scheduler/internal/garbage-thread.hpp>

namespace OS {

class Task;
class State;

class Thread : public SchedThread, public GarbageThread::Garbage {
public:
  // next 4 methods are all @noncritical
  static Thread * NewUser(Task * owner, void * call);
  static Thread * NewUser(Task * owner, void * call, void * arg);
  static Thread * NewKernel(Task * owner, void * call);
  static Thread * NewKernel(Task * owner, void * call, void * arg);
  
  ~Thread();
  
  virtual void Delete(); // @noncritical
  Task * GetTask(); // @ambicritical
  State & GetState();
  
  uint64_t GetThreadId();
  
  uint64_t timeoutLock OS_ALIGNED(8) = 0; // @noncritical
  bool shouldClearTimeout = false;
  bool isSleeping = false;
  
protected:
  Thread * taskNext, * taskLast;
  friend class Task;
  
  virtual void CleanupGarbage();
  
  uint64_t threadId;
  
  Thread(Task * owner, bool kernel, void * func); // @noncritical
  Thread(Task * owner, bool kernel, void * func, void * arg); // @noncritical
  
private:
  Task * task;
  State * state;
};

}

#endif
