#ifndef __SCHEDULER_THREAD_HPP__
#define __SCHEDULER_THREAD_HPP__

#include <scheduler-specific/thread.hpp>

namespace OS {

class Task;
class State;
class GarbageThread;

class Thread : public SchedThread {
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
  
protected:
  Thread * taskNext, * taskLast;
  Thread * garbageNext;
  friend class Task;
  friend class GarbageThread;
  
  Thread(Task * owner, bool kernel, void * func); // @noncritical
  Thread(Task * owner, bool kernel, void * func, void * arg); // @noncritical
  
private:
  Task * task;
  State * state;
};

}

#endif
