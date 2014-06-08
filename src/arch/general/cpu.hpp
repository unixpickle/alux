#ifndef __GENERAL_CPU_HPP__
#define __GENERAL_CPU_HPP__

#include <multitasking/thread.hpp>
#include <scheduler/user-info.hpp>
#include <cstddef>
#include <cstdint>
#include <common>

namespace OS {

class CPU {
public:
  static void HaltCurrent();
  static CPU & GetCurrent();
  static void WaitUntilTick(); // @critical -> @noncritical (no-return)
  static void SetTimeout(uint64_t clockTicks);
  
  static size_t GetCount();
  static CPU & GetAt(size_t idx);
  
  virtual size_t GetIndex() = 0; // @ambicritical
  virtual void Wakeup() = 0; // @critical
  virtual Thread * GetThread(); // @critical; doesn't retain/release group
  virtual void SetThread(Thread *); // @critical; consumes ownership
  
  Scheduler::UserInfo * userInfo;
  
protected:
  uint64_t threadLock OS_ALIGNED(8); // @critical
  Thread * thread;
};

}

#endif
