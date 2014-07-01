#ifndef __SCHEDULER_GARBAGE_THREAD_HPP__
#define __SCHEDULER_GARBAGE_THREAD_HPP__

#include <module/module.hpp>
#include <cstdint>
#include <macros>

namespace OS {

class Thread;

class GarbageThread : public Module {
public:
  class Garbage {
  protected:
    friend class GarbageThread;
    
    Garbage * garbageNext;
    
    virtual void CleanupGarbage() = 0;
  };
  
  static void InitGlobal(); // @noncritical
  static GarbageThread & GetGlobal(); // @ambicritical
  
  Thread * GetThread();
  
  void Push(Garbage * garbage); // @ambicritical
  
protected:
  virtual void Initialize();
  virtual DepList GetDependencies();
  
private:
  uint64_t lock OS_ALIGNED(8) = 0;
  Garbage * first = NULL;
  
  Thread * thread;
  
  static void CallMain(); // @noncritical
  
  void Wakeup(); // @critical
  void Main(); // @noncritical
  Garbage * Pop(); // @ambicritical, unsynchronized
};

}

#endif
