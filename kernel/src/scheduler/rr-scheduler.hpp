#ifndef __ALUX_RR_SCHEDULER_HPP__
#define __ALUX_RR_SCHEDULER_HPP__

#include "scheduler.hpp"
#include <ansa/atomic>

namespace Alux {

/**
 * This is a single-queue round-robin scheduler with timer support. This is not
 * particularly optimized for SMP systems, although it will work on them.
 */
class RRScheduler : public Scheduler {
public:
  static const uint64_t JiffyUs = 50000;
  
  RRScheduler(); // @noncritical
  
  virtual void Add(Thread &);
  virtual void Remove(Thread &);
  
  virtual void SetTimeout(uint64_t deadline);
  virtual void SetTimeout(uint64_t deadline, ansa::Lock & unlock);
  virtual void SetInfiniteTimeout();
  virtual void SetInfiniteTimeout(ansa::Lock & unlock);
  
  virtual void ClearTimeout(Thread &);
  virtual void Yield();
  
  virtual GarbageCollector & GetGarbageCollector();
  virtual void Run();
  
protected:
  virtual void SetGarbageTimeout();
  virtual void ClearGarbageTimeout();
  
private:
  struct ThreadObj {
    inline ThreadObj(Thread & t) : link(*this), thread(t) {}
    
    ansa::LinkedList<ThreadObj>::Link link;
    Thread & thread;
    ansa::Atomic<uint64_t> deadline;
    bool running = false;
  };
  
  anarch::CriticalLock lock;
  ansa::LinkedList<ThreadObj> threads;
  
  GarbageCollector collector;
  
  void Switch(); // @critical
  void ResignCurrent(); // @critical, unsynchronized
  
  static void CallSwitch(void * scheduler);
  static void SuspendAndSwitch(void * scheduler);
  static void RunSyncAndSwitch(void * scheduler);
};

}

#endif
