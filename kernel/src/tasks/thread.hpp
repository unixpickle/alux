#ifndef __ALUX_THREAD_HPP__
#define __ALUX_THREAD_HPP__

#include "../scheduler/garbage-object.hpp"
#include "../identifiers/id-map.hpp"
#include <anarch/api/state>
#include <anarch/lock>
#include <ansa/linked-list>

namespace Alux {

class Task;

class Thread : public GarbageObject {
public:
  static Thread * GetCurrent(); // @ambicritical
  static void SetCurrent(Thread *); // @critical
  static void ExitCurrent(); // @ambicritical
  
  /**
   * Create a new thread on a given task. The task must be retained. If a
   * thread could not be allocated, NULL will be returned. Otherwise, a
   * retained thread will be returned. The thread will automatically be added
   * to its owning task and scheduled on the task's scheduler.
   * @noncritical
   */
  Thread * New(Task &, anarch::State &);
  
  Task & GetTask() const; // @ambicritical
  anarch::State & GetState() const; // @ambicritical
  
  bool Retain(); // @ambicritical
  void Release(); // @ambicritical
  
  Identifier GetIdentifier() const; // @ambicritical
  
  virtual void Dealloc(); // @noncritical
  
protected:
  friend class IdMap<Thread>;
  ansa::LinkedList<Thread>::Link idMapLink;
  
  friend class Scheduler;
  void * schedulerUserInfo;
  
private:
  Thread(Task &, anarch::State &);
  bool Init();
  void Deinit();
  
  Task & task;
  anarch::State & state;
  
  Identifier identifier;
  
  anarch::CriticalLock lifeLock;
  int retainCount = 1;
  bool killed = false;
};

}

#endif
