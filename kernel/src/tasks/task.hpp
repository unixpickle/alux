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
  virtual anarch::MemoryMap & GetMemoryMap() const = 0;
  virtual bool IsUserTask() const = 0;
  bool Retain();
  void Release();
  bool Hold();
  void Unhold();
  void Kill(uint16_t status);
  Scheduler & GetScheduler() const;
  Identifier GetIdentifier() const;
  
protected:
  Task(Scheduler &);
  virtual void Init(); // @noncritical
  virtual void Dealloc(); // @noncritical
  
  friend class IdMap<Task>;
  ansa::LinkedList<Task>::Link idMapLink;
};

}

#endif
