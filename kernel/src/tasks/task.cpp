#include "task.hpp"
#include "../scheduler/scheduler.hpp"
#include <anarch/critical>
#include <anarch/new>
#include <ansa/atomic>

namespace OS {

namespace {

ansa::Atomic<uint64_t> counter;

}

void Task::InitializeCounter() {
  new(&counter) ansa::Atomic<uint64_t>(0);
}

uint64_t Task::ReadCounter() {
  return counter;
}

bool Task::Retain() {
  anarch::ScopedCritical critical;
  anarch::ScopedLock lock(stateLock);
  if (isKilled && !holdCount) return false;
  ++retainCount;
  return true;
}

void Task::Release() {
  anarch::ScopedCritical critical;
  stateLock.Seize();
  bool terminated = !--retainCount && !holdCount && isKilled;
  stateLock.Release();
  if (terminated) ThrowAway();
}

bool Task::Hold() {
  anarch::ScopedCritical critical;
  anarch::ScopedLock lock(stateLock);
  if (isKilled) return false;
  ++holdCount;
  return true;
}

void Task::Unhold() {
  anarch::ScopedCritical critical;
  stateLock.Seize();
  bool terminated = !--holdCount && !retainCount && isKilled;
  stateLock.Release();
  if (terminated) ThrowAway();
}

void Task::Kill(uint16_t status) {
  anarch::ScopedCritical critical;
  anarch::ScopedLock lock(stateLock);
  assert(retainCount || holdCount);
  if (!isKilled) {
    isKilled = true;
    killStatus = status;
  }
}

Scheduler & Task::GetScheduler() const {
  return scheduler;
}

TaskId Task::GetId() const {
  return identifier;
}

ThreadId Task::AddThread(Thread & th) {
  AssertNoncritical();
  anarch::ScopedLock scope(threadsLock);
  threads.Add(&th.taskLink);
  return threadCounter++;
}

void Task::RemoveThread(Thread & th) {
  AssertNoncritical();
  anarch::ScopedLock scope(threadsLock);
  threads.Remove(&th.taskLink);
}

Thread * Task::LookupThread(ThreadId theId) {
  AssertNoncritical();
  anarch::ScopedLock scope(threadsLock);
  auto iter = threads.GetStart();
  auto iterEnd = threads.GetEnd();
  while (iter != iterEnd) {
    if ((*iter).GetId() == theId) {
      return &(*iter);
    }
    ++iter;
  }
  return NULL;
}

Task::Task(Scheduler & s)
  : GarbageObject(s.GetGarbageCollector()), pidPoolLink(*this), scheduler(s) {
  ++counter;
}

Task::~Task() {
  --counter;
}

void Task::Deinit() {
  Thread * t;
  while ((t = threads.Pop())) {
    if (t->isScheduled) {
      GetScheduler().Remove(*t);
    }
    t->ThrowAwaySync();
  }
  GetScheduler().GetTaskIdPool().Free(*this);
}

void Task::Init() {
  identifier = GetScheduler().GetTaskIdPool().Alloc(*this);
}

}
