#ifndef __ALUX_THREAD_HPP__
#define __ALUX_THREAD_HPP__

#include "sleep-state.hpp"
#include "../scheduler/garbage-object.hpp"
#include "../identifiers/id-map.hpp"
#include <anarch/api/state>
#include <anarch/lock>
#include <ansa/linked-list>

namespace Alux {

class Task;
class HoldScope;

class Thread : public GarbageObject {
public:
  virtual ~Thread();
  
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
  static Thread * New(Task &, anarch::State &);
  
  Task & GetTask() const; // @ambicritical
  anarch::State & GetState() const; // @ambicritical
  SleepState & GetSleepState(); // @ambicritical
  
  bool Retain(); // @ambicritical
  void Release(); // @ambicritical
  void Kill(); // @ambicritical
  
  Identifier GetIdentifier() const; // @ambicritical
  
  virtual void Dealloc(); // @noncritical
  
protected:
  template <class T, int C>
  friend class IdMap;
  
  ansa::LinkedList<Thread>::Link idMapLink;
  
  friend class Scheduler;
  void * schedulerUserInfo;
  
  friend class HoldScope;
  bool holdingTask = false;
  
  SleepState sleepState;
  
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
