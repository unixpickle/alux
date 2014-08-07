#include "task.hpp"
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

Task::Task() {
  ++counter;
}

Task::~Task() {
  --counter;
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
  if (terminated) PushGarbage();
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
  if (terminated) PushGarbage();
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

void Task::RemoveReferences() {
  // TODO: here, remove our ref from the PID pool and from the Scheduler
}

void Task::PushGarbage() {
  // TODO: push us to some sort of GarbageThread
}

}
