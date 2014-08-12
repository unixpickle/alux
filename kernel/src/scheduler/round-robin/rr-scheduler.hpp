#ifndef __ALUX_ROUNDROBIN_SCHEDULER_HPP__
#define __ALUX_ROUNDROBIN_SCHEDULER_HPP__

#include "thread-info.hpp"
#include "../scheduler.hpp"
#include "../../memory/garbage-thread.hpp"
#include "../../tasks/kernel-task.hpp"

namespace OS {

class RRScheduler : public Scheduler {
public:
  static uint64_t JiffyUs = 2000;
  
  virtual void Init(); // @noncritical
  
  virtual void Add(Thread & th);
  virtual void Remove(Thread & th);
  virtual void RemoveAll(Task & task);
  virtual void SetTimeout(uint64_t deadline, ansa::Lock & unlock);
  virtual void SetInfiniteTimeout(ansa::Lock & unlock);
  virtual void ClearTimeout(Thread & thread);
  virtual void Yield();
  virtual GarbageCollector & GetGarbageCollector() const;
  virtual TaskIdPool & GetTaskIdPool() const;
  
protected:
  KernelTask * kernelTask;
  Thread * garbageThread;
  GarbageCollector * garbageCollector;
  IncrementalIdPool pidPool;
  
  anarch::CriticalLock lock;
  ansa::LinkedList<Thread> threads;
  
private:
  static void StartGarbage(void * schedPtr);
  static void CallSwitch(void * obj);
  
  void Switch();
};

}

#endif
