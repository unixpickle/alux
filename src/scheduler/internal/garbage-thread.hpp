#ifndef __SCHEDULER_GARBAGE_THREAD_HPP__
#define __SCHEDULER_GARBAGE_THREAD_HPP__

#include <module/module.hpp>
#include <scheduler/general/thread.hpp>

namespace OS {

class GarbageThread : public Module {
public:
  static void InitGlobal(); // @noncritical
  static GarbageThread & GetGlobal(); // @ambicritical
  
  Thread * GetThread();
  
  void PushTask(Task * t); // @critical
  
protected:
  virtual void Initialize();
  virtual DepList GetDependencies();
  
private:
  uint64_t tasksLock OS_ALIGNED(8) = 0; // @critical
  Task * firstTask = NULL;
  
  Thread * thread;
  
  static void CallMain();
  
  void Wakeup(); // @critical
  void Main(); // @noncritical
  Task * PopTask(); // @critical
};

}

#endif
