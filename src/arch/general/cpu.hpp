#ifndef __GENERAL_CPU_HPP__
#define __GENERAL_CPU_HPP__

#include <scheduler/base/thread.hpp>
#include <cstdint>
#include <common>

namespace OS {

class CPU {
public:
  static CPU & GetCurrent(); // @critical
  static size_t GetCount(); // @ambicritical
  static CPU & GetAt(size_t index); // @ambicritical
  static void SaveAndTick(); // @critical
  static void SetTimeout(uint64_t delay, bool precision); // @critical
  static void ClearTimeout(); // @critical
  static void Sleep(); // @critical -> @noncritical (no return)
  static void SetThread(Thread * th); // @critical
  
  virtual size_t GetIndex() = 0; // @ambicritical
  virtual void Wake() = 0; // @critical
  virtual Thread * GetThread(); // @critical
  
protected:
  uint64_t threadLock OS_ALIGNED(8);
  Thread * thread;
};

}

#endif
