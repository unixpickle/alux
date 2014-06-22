#ifndef __GENERAL_HARDWARE_THREAD_HPP__
#define __GENERAL_HARDWARE_THREAD_HPP__

#include <scheduler/general/thread.hpp>
#include <cstdint>
#include <macros>

namespace OS {

class HardwareThread {
public:
  static void SetThread(Thread * th); // @critical
  static Thread * GetThread();
  
  static HardwareThread & GetCurrent();
  
  virtual void Wake() = 0; // @critical
  
protected:
  virtual void SetCurrentThread(Thread * th);
  virtual Thread * GetCurrentThread();
  
  uint64_t threadLock OS_ALIGNED(8);
  Thread * thread;
};

}

#endif

