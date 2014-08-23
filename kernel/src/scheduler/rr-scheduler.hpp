#ifndef __ALUX_RR_SCHEDULER_HPP__
#define __ALUX_RR_SCHEDULER_HPP__

#include "scheduler.hpp"

namespace Alux {

/**
 * This is a single-queue round-robin scheduler with timer support. This is not
 * particularly optimized for SMP systems, although it will work on them.
 */
class RRScheduler : public Scheduler {
public:
  RRScheduler(); // @noncritical
  
  virtual void Add(Thread &);
  virtual void Remove(Thread &);
  
  virtual void SetTimeout(uint64_t deadline);
  virtual void SetTimeout(uint64_t deadline, ansa::Lock & unlock);
  virtual void SetInfiniteTimeout();
  virtual void SetInfiniteTimeout(ansa::Lock & unlock);
  
  virtual void ClearTimeout(Thread &);
  virtual void Yield();
  
  virtual GarbageCollector & GetGarbageCollector() const;
  
private:
  struct ThreadObj {
    inline ThreadObj(Thread & t) : link(*this), thread(t) {}
    
    ansa::LinkedList<ThreadObj>::Link link;
    Thread & thread;
  };
  
  ansa::LinkedList<ThreadObj> threads;
};

}

#endif
