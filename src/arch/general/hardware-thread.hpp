#ifndef __GENERAL_HARDWARE_THREAD_HPP__
#define __GENERAL_HARDWARE_THREAD_HPP__

#include <scheduler/general/thread.hpp>
#include <cstdint>
#include <macros>

namespace OS {

class HardwareThread {
public:
  static void SetThread(Thread * th); // @critical
  static HardwareThread & GetCurrent();
  
  virtual void Wake() = 0; // @critical
  virtual Thread * GetThread(); // @critical
  virtual bool IsRunningTask(Task *);
  
protected:
  virtual void SetCurrentThread(Thread * th);
  
  uint64_t threadLock OS_ALIGNED(8);
  Thread * thread;
};

}

#endif

