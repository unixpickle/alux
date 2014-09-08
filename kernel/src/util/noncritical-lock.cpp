#include "noncritical-lock.hpp"
#include "../scheduler/scheduler.hpp"
#include <anarch/api/domain-list>
#include <anarch/critical>

namespace Alux {

void NoncriticalLock::Seize() {
  AssertNoncritical();
  
  queueLock.Seize();
  if (!held) {
    held = true;
    queueLock.Release();
    return;
  }
  
  WaitingThread threadLink;
  
  Thread * th = NULL;
  if (anarch::DomainList::GetGlobal().IsInitialized()) {
    th = Thread::GetCurrent();
    if (th) {
      bool result = th->Retain();
      assert(result);
      (void)result;
    }
  }
  
  if (!th) {
    queue.Add(&threadLink.link);
    queueLock.Release();
    while (!threadLink.owner) {}
  } else {
    threadLink.thread = th;
    queue.Add(&threadLink.link);
    th->GetTask().GetScheduler().SetInfiniteTimeout(queueLock);
    
    // some assert macros might not like the implied cast without "== true"
    assert(threadLink.owner == true);
    th->Release();
  }
}

void NoncriticalLock::Release() {
  AssertNoncritical();
  queueLock.Seize();
  WaitingThread * next = queue.Shift();
  if (!next) {
    held = false;
    queueLock.Release();
    return;
  }
  queueLock.Release();
  
  // we must read next->thread *before* we set the `owner` field for the case
  // where thread is NULL and some CPU is polling the `owner` field.
  if (next->thread) {
    next->owner = true;
    next->thread->GetTask().GetScheduler().ClearTimeout(*next->thread);
  } else {
    next->owner = true;
  }
}

}
