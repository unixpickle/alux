#ifndef __ALUX_THREAD_HPP__
#define __ALUX_THREAD_HPP__

#include <anarch/api/state>
#include <anarch/lock>
#include <ansa/nocopy>

namespace OS {

class Task;

class Thread : public ansa::NoCopy {
public:
  static void InitializeCounter(); // @ambicritical
  static uint64_t ReadCounter(); // @ambicritical
  
  Thread(Task & task, anarch::State & state); // @noncritical
  ~Thread(); // @noncritical
  
  Task & GetTask();
  anarch::State & GetState();
  
  bool Retain(); // @ambicritical
  void Release(); // @ambicritical
  
private:
  Task & task;
  anarch::State & state;
  
  anarch::CriticalLock retainLock;
  int retainCount = 1;
  bool isKilled = false;
  
  void PushGarbage();
};

}

#endif
