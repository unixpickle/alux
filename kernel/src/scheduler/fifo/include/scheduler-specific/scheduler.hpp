#ifndef __FIFO_SCHEDULER_HPP__
#define __FIFO_SCHEDULER_HPP__

#include <scheduler/general/thread.hpp>
#include <module/module.hpp>
#include <macros>
#include <cstdint>

namespace OS {

class GarbageThread;

class Scheduler : public Module {
public:
  static const uint64_t Jiffy = 6000; // 100Hz
  
  static void InitGlobal();
  static Scheduler & GetGlobal();
  
  void Start(); // @noncritical
  
  // operations acting on the scheduler as a whole
  
  void AddThread(Thread *); // @critical

  // operations acting on the current thread or a specific thread
  
  /**
   * Set a timeout.
   * @param deadline The deadline after which the timeout should end
   * @param precise Whether precision is required with this timeout
   * @param unlock If non-null, this anlock will be unlocked after the timeout
   * has been set.
   * @critical
   */
  void SetTimeout(uint64_t deadline, bool precise, uint64_t * unlock = NULL);
  void SetInfiniteTimeout(uint64_t * unlock = NULL); // @critical
  bool ClearTimeout(Thread *); // @critical
  void Resign(); // @critical

  /**
   * Called by the architecture when a CPU timer fires or when the scheduler
   * requests a blank slate
   * @critical
   */
  void Tick();
  
protected:
  friend class Task;
  friend class Thread;
  void RemoveThread(Thread *); // @critical
  
private:
  uint64_t lock OS_ALIGNED(8) = 0;
  Thread * firstThread = NULL;
  Thread * lastThread = NULL;
  
  virtual DepList GetDependencies();
  
  void SwitchThread(Thread *); // @critical
  Thread * GetNextThread(uint64_t & nextDelay); // @critical
  Thread * PopThread(); // @critical, nosync
  void PushThread(Thread *); // @critical, nosync
  void UnlinkThread(Thread *);
};

}

#endif