#ifndef __X64_SCHEDULER_EXPERT_HPP__
#define __X64_SCHEDULER_EXPERT_HPP__

#include <arch/general/scheduler-expert.hpp>

namespace OS {

namespace x64 {

class SchedulerExpert : public OS::SchedulerExpert {
public:
  static void Initialize();
  static SchedulerExpert & GetGlobal();
  
  virtual OS::Task * CreateKernelTask();
  virtual OS::Thread * CreateKernelThread(OS::Task * kernTask, void * func);
  virtual OS::Thread * CreateKernelThread(OS::Task * kernTask, void * func,
                                          void * argument);
  virtual void SaveAndTick();
  virtual void SetTimeout(uint64_t delay, bool precision);
  virtual void ClearTimeout();
  virtual void WaitTimeout();
};

void LapicTickMethod();

}

}

#endif
