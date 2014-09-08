#include "sleep-state.hpp"
#include "../scheduler/scheduler.hpp"
#include "../util/time.hpp"
#include <anarch/critical>

namespace Alux {

void SleepState::Unsleep(Thread & th) {
  th.sleepState.Cancel();
}

SleepState::SleepState(Thread & t) : thread(t) {
}

void SleepState::Sleep(uint64_t nanos) {
  AssertCritical();
  
  Thread * th = Thread::GetCurrent();
  assert(th != NULL);
  SleepState & state = th->sleepState;
  
  state.lock.Seize();
  
  if (state.cancelled || !nanos) {
    state.cancelled = false;
    state.lock.Release();
    return;
  }
  
  state.sleeping = true;
  th->GetTask().GetScheduler().SetTimeout(NanosFromNow(nanos), state.lock);
  
  anarch::ScopedLock scope(state.lock);
  state.sleeping = false;
  state.cancelled = false;
}

void SleepState::SleepInfinite() {
  AssertCritical();
  
  Thread * th = Thread::GetCurrent();
  assert(th != NULL);
  SleepState & state = th->sleepState;
  
  state.lock.Seize();
  
  if (state.cancelled) {
    state.cancelled = false;
    state.lock.Release();
    return;
  }
  
  state.sleeping = true;
  th->GetTask().GetScheduler().SetInfiniteTimeout(state.lock);
  
  anarch::ScopedLock scope(state.lock);
  state.sleeping = false;
  state.cancelled = false;
}

void SleepState::Cancel() {
  anarch::ScopedCritical critical;
  anarch::ScopedLock scope(lock);
  if (!sleeping) {
    cancelled = true;
  } else {
    thread.GetTask().GetScheduler().ClearTimeout(thread);
  }
}

}
