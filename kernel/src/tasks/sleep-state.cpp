#include "sleep-state.hpp"
#include "thread.hpp"
#include "task.hpp"
#include "../scheduler/scheduler.hpp"
#include <anarch/api/clock-module>
#include <anarch/api/clock>
#include <anarch/critical>

namespace Alux {

namespace {

uint64_t NanoDeadline(uint64_t nanos) {
  anarch::Clock & clock = anarch::ClockModule::GetGlobal().GetClock();
  uint64_t ticks = clock.GetTicks();
  return ticks + clock.GetMicrosPerTick().Flip().ScaleInteger(nanos / 1000);
}

}

void SleepState::Cancel() {
  AssertCritical();
  anarch::ScopedLock scope(lock);
  if (!sleeping) {
    cancelled = true;
  } else {
    Thread * th = Thread::GetCurrent();
    assert(th != NULL);
    th->GetTask().GetScheduler().ClearTimeout(*th);
  }
}

void SleepState::Sleep(uint64_t nanos) {
  AssertCritical();
  
  Thread * th = Thread::GetCurrent();
  assert(th != NULL);
  SleepState & state = th->GetSleepState();
  
  state.lock.Seize();
  
  if (state.cancelled || !nanos) {
    state.cancelled = false;
    state.lock.Release();
    return;
  }
  
  state.sleeping = true;
  th->GetTask().GetScheduler().SetTimeout(NanoDeadline(nanos), state.lock);
  
  anarch::ScopedLock(state.lock);
  state.sleeping = false;
  state.cancelled = false;
}

void SleepState::SleepInfinite() {
  AssertCritical();
  
  Thread * th = Thread::GetCurrent();
  assert(th != NULL);
  SleepState & state = th->GetSleepState();
  
  state.lock.Seize();
  
  if (state.cancelled) {
    state.cancelled = false;
    state.lock.Release();
    return;
  }
  
  state.sleeping = true;
  th->GetTask().GetScheduler().SetInfiniteTimeout(state.lock);
  
  anarch::ScopedLock(state.lock);
  state.sleeping = false;
  state.cancelled = false;
}

}
