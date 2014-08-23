#ifndef __ALUX_TASK_HPP__
#define __ALUX_TASK_HPP__

#include "thread.hpp"
#include "../scheduler/garbage-object.hpp"
#include "../identifiers/id-map.hpp"
#include <anarch/api/memory-map>

namespace Alux {

class Scheduler;

class Task : public GarbageObject {
public:
  // both @ambicritical
  virtual anarch::MemoryMap & GetMemoryMap() const = 0;
  virtual bool IsUserTask() const = 0;
  
  // all @ambicritical
  bool Retain();
  void Release();
  bool Hold();
  void Unhold();
  void Kill(uint16_t status);
  Scheduler & GetScheduler() const;
  Identifier GetIdentifier() const;
  IdMap<Thread> & GetThreads();
  
protected:
  Task(Scheduler &); // @ambicritical
  virtual bool Init(); // @noncritical
  virtual void Deinit(); // @noncritical
  
  friend class IdMap<Task>;
  ansa::LinkedList<Task>::Link idMapLink;
  
  friend class Scheduler;
  void * schedulerUserInfo;
  
private:
  Scheduler & scheduler;
  Identifier identifier;
  IdMap<Thread> threads;
  
  anarch::CriticalLock lifeLock;
  int retainCount = 0;
  int holdCount = 1;
  uint16_t killStatus;
  bool killed = false;
};

}

#endif
