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
  void PushThread(Thread * t); // @critical
  
protected:
  virtual void Initialize();
  virtual DepList GetDependencies();
  
private:
  uint64_t lock OS_ALIGNED(8) = 0;
  Task * firstTask = NULL;
  Thread * firstThread = NULL;
  
  Thread * thread;
  
  static void CallMain();
  
  void Wakeup(); // @critical
  void Main(); // @noncritical
  Task * PopTask(); // @ambicritical, unsynchronized
  Thread * PopThread(); // @ambicritical, unsynchronized
};

}

#endif
