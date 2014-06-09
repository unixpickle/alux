#ifndef __FIFO_SCHEDULER_HPP__
#define __FIFO_SCHEDULER_HPP__

#include <scheduler/base/thread.hpp>
#include <common>
#include <cstdint>

namespace OS {

class Scheduler {
public:
  static const uint64_t Jiffy = 60000; // 100Hz
  
  static void Initialize();
  static Scheduler & GetGlobal();
  
  // operations acting on the scheduler as a whole
  
  void AddThread(Thread *); // @critical
  void RemoveThread(Thread *); // @critical

  // operations acting on the current thread or a specific thread
  
  void SetTimeout(uint64_t deadline, bool precise); // @critical
  void SetInfiniteTimeout(); // @critical
  bool ClearTimeout(Thread *); // @critical
  void Resign(); // @critical

  /**
   * Called by the architecture when a CPU timer fires or when the scheduler
   * requests a blank slate
   * @critical
   */
  void Tick();
  
protected:
  uint64_t lock OS_ALIGNED(8) = 0;
  Thread * firstThread = NULL;
  Thread * lastThread = NULL;
  
  Thread * PopThread(); // @critical, nosync
  void PushThread(Thread *); // @critical, nosync
  void UnlinkThread(Thread *);
};

}

#endif
