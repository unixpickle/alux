#ifndef __GENERAL_HARDWARE_THREAD_HPP__
#define __GENERAL_HARDWARE_THREAD_HPP__

#include <scheduler/general/thread.hpp>
#include <cstdint>
#include <common>

namespace OS {

class HardwareThread {
public:
  static void SetThread(Thread * th); // @critical
  static HardwareThread & GetCurrent();
  
  virtual int GetIndex() = 0; // @ambicritical
  virtual void Wake() = 0; // @critical
  virtual Thread * GetThread(); // @critical
  virtual bool IsRunningTask(Task *);
  
protected:
  uint64_t threadLock OS_ALIGNED(8);
  Thread * thread;
};

}

#endif
