#ifndef __ALUX_TASK_HPP__
#define __ALUX_TASK_HPP__

#include <anarch/lock>
#include <anarch/api/user-map>
#include <ansa/nocopy>

namespace OS {

class Task : public ansa::NoCopy {
public:
  static void InitializeCounter(); // @ambicritical
  static uint64_t ReadCounter(); // @ambicritical
  
  Task(); // @noncritical
  virtual ~Task(); // @noncritical
  
  virtual anarch::MemoryMap * GetMemoryMap() = 0; // @ambicritical
  virtual bool IsUserTask() = 0; // @ambicritical
  
  bool Retain(); // @ambicritical
  void Release(); // @ambicritical
  bool Hold(); // @ambicritical
  void Unhold(); // @ambicritical
  void Kill(uint16_t killStatus); // @ambicritical
  
protected:
  virtual void RemoveReferences();
  
private:
  anarch::CriticalLock stateLock;
  int retainCount = 0;
  int holdCount = 1;
  uint16_t killStatus;
  bool isKilled = false;
  
  void PushGarbage();
};

}

#endif
