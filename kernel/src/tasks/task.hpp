#ifndef __ALUX_TASK_HPP__
#define __ALUX_TASK_HPP__

#include "thread.hpp"
#include "../scheduler/garbage-object.hpp"
#include "../identifiers/pool-id-map.hpp"
#include <anarch/api/memory-map>

namespace Alux {

class Scheduler;

class Task : public GarbageObject {
public:
  typedef PoolIdMap<Thread, 0x10> ThreadMap;
  
  static const int KillReasonNormal = 0;
  static const int KillReasonAbort = 1;
  static const int KillReasonPermissions = 2;
  
  // both @ambicritical
  virtual anarch::MemoryMap & GetMemoryMap() const = 0;
  virtual bool IsUserTask() const = 0;
  
  // all @ambicritical
  bool Retain();
  void Release();
  bool Hold();
  void Unhold();
  void Kill(int status);
  Scheduler & GetScheduler() const;
  Identifier GetIdentifier() const;
  Identifier GetUserIdentifier() const;
  ThreadMap & GetThreads();
  
protected:
  Task(Identifier uid, Scheduler &); // @ambicritical
  virtual bool Init(); // @noncritical
  virtual void Deinit(); // @noncritical
  
  template <class T, int C>
  friend class IdMap;
  
  ansa::LinkedList<Task>::Link idMapLink;
  
private:
  Identifier uid;
  
  Scheduler & scheduler;
  Identifier identifier;
  ThreadMap threads;
  
  anarch::CriticalLock lifeLock;
  int retainCount = 0;
  int holdCount = 1;
  int killStatus;
  bool killed = false;
};

}

#endif
