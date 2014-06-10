#ifndef __SCHEDULER_GARBAGE_THREAD_HPP__
#define __SCHEDULER_GARBAGE_THREAD_HPP__

#include <scheduler/general/thread.hpp>

namespace OS {

class GarbageThread : public Thread {
public:
  static void Initialize(Task * kernelTask); // @noncritical
  static GarbageThread & GetGlobal(); // @ambicritical
  
  GarbageThread(); // DO NOT CALL
  
  virtual void Delete(); // @noncritical (don't call it, bro)
  void PushTask(Task * t); // @critical
  
private:
  uint64_t tasksLock OS_ALIGNED(8) = 0; // @critical
  Task * firstTask = NULL;
  
  static void Wakeup(); // @critical
  static void CallMain();
  
  GarbageThread(Task * owner); // @noncritical
  
  void Main(); // @noncritical
  Task * PopTask(); // @critical
};

}

#endif
