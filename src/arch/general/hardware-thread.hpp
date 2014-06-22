#ifndef __GENERAL_HARDWARE_THREAD_HPP__
#define __GENERAL_HARDWARE_THREAD_HPP__

#include <scheduler/general/thread.hpp>
#include <cstdint>
#include <macros>
#include <atomic>

namespace OS {

class HardwareThread {
public:
  static void SetThread(Thread * th); // @critical
  static Thread * GetThread();
  
  static HardwareThread & GetCurrent();
  
  HardwareThread();
  virtual void Wake() = 0; // @critical
  
protected:
  Atomic<Thread *> thread;
};

}

#endif

