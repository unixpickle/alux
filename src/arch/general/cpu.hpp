#ifndef __GENERAL_CPU_HPP__
#define __GENERAL_CPU_HPP__

#include <scheduler/base/thread.hpp>
#include <cstdint>
#include <common>

namespace OS {

class CPU {
public:
  static void SetThread(Thread * th); // @critical
  static CPU & GetCurrent();
  
  virtual int GetIndex() = 0; // @ambicritical
  virtual void Wake() = 0; // @critical
  virtual Thread * GetThread(); // @critical
  
protected:
  uint64_t threadLock OS_ALIGNED(8);
  Thread * thread;
};

}

#endif
