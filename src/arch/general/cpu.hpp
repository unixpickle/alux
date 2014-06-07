#ifndef __GENERAL_CPU_HPP__
#define __GENERAL_CPU_HPP__

#include <scheduler/types.hpp>

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
  virtual Scheduler::Job * GetJob() = 0; // @critical
  
  Scheduler::UserInfo * userInfo;
};

}

#endif
