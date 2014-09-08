// no need to include "thread.hpp" or "task.hpp" ourselves
#include "../scheduler/scheduler.hpp"
#include <anarch/critical>

namespace Alux {

Task::Task(Identifier user, Scheduler & sched)
  : GarbageObject(sched.GetGarbageCollector()), hashMapLink(*this),
    uid(user), scheduler(sched) { 
}

bool Task::AddToScheduler() {
  if (!scheduler.GetTaskList().Add(*this)) {
    return false;
  }
  return inScheduler = true;
}

bool Task::Retain() {
  anarch::ScopedCritical critical;
  anarch::ScopedLock scope(lifeLock);
  if (killReason != -1 && !holdCount) return false;
  ++retainCount;
  return true;
}

void Task::Release() {
  anarch::ScopedCritical critical;
  anarch::ScopedLock scope(lifeLock);
  --retainCount;
  if (!retainCount && !holdCount && killReason != -1) {
    // by the time we've gotten to this point, no other method should be
    // running on the task so we don't have to worry about throwing it away
    ThrowAway();
  }
}

bool Task::Hold() {
  anarch::ScopedCritical critical;
  anarch::ScopedLock scope(lifeLock);
  if (killReason != -1) return false;
  ++holdCount;
  return true;
}

void Task::Unhold() {
  anarch::ScopedCritical critical;
  anarch::ScopedLock scope(lifeLock);
  --holdCount;
  if (!retainCount && !holdCount && killReason != -1) {
    // see note in [Release]
    ThrowAway();
  }
}

void Task::Kill(int reason) {
  anarch::ScopedCritical critical;
  anarch::ScopedLock scope(lifeLock);
  if (killReason != -1) return;
  killReason = reason;
}

int Task::GetKillReason() {
  anarch::ScopedLock scope(lifeLock);
  return killReason;
}

void Task::Dealloc() {
  for (int i = 0; i < threadList.GetMap().GetBucketCount(); ++i) {
    auto & bucket = threadList.GetMap().GetBucket(i);
    for (auto j = bucket.GetStart(); j != bucket.GetEnd(); ++j) {
      Thread & th = *j;
      th.Dealloc();
    }
  }
  if (inScheduler) {
    scheduler.GetTaskList().Remove(*this);
  }
}

}
