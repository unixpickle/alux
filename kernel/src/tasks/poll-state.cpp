#include "poll-state.hpp"
#include "task.hpp"
#include "../scheduler/scheduler.hpp"
#include "../ipc/user-port.hpp"
#include "../util/time.hpp"
#include <anarch/critical>

namespace Alux {

PollState::PollState(Thread & t) : thread(t) {
}

void PollState::Signal(UserPort & port) {
  anarch::ScopedCritical critical;
  anarch::ScopedLock scope(lock);
  if (!polling) return;
  if (specific && specific != &port) return;
  thread.GetTask().GetScheduler().ClearTimeout(thread);
}

void PollState::Poll() {
  AssertCritical();
  
  // get current state
  Thread * th = Thread::GetCurrent();
  assert(th != NULL);
  PollState & state = th->GetPollState();
  
  state.PollSpecific(NULL, NULL);
}

void PollState::Poll(UserPort & port) {
  AssertCritical();
  
  // get current state
  Thread * th = Thread::GetCurrent();
  assert(th != NULL);
  PollState & state = th->GetPollState();
  
  state.PollSpecific(&port, NULL);
}

void PollState::Poll(uint64_t nanoseconds) {
  AssertCritical();
  
  // get current state
  Thread * th = Thread::GetCurrent();
  assert(th != NULL);
  PollState & state = th->GetPollState();
  
  state.PollSpecific(NULL, &nanoseconds);
}

void PollState::Poll(UserPort & port, uint64_t nanoseconds) {
  AssertCritical();
  
  // get current state
  Thread * th = Thread::GetCurrent();
  assert(th != NULL);
  PollState & state = th->GetPollState();
  
  state.PollSpecific(&port, &nanoseconds);
}

void PollState::PollSpecific(UserPort * s, uint64_t * nanoseconds) {
  // begin polling
  lock.Seize();
  assert(!polling);
  polling = true;
  specific = s;
  if (nanoseconds) {
    thread.GetTask().GetScheduler().SetTimeout(NanosFromNow(*nanoseconds),
                                               lock);
  } else {
    thread.GetTask().GetScheduler().SetInfiniteTimeout(lock);
  }
  
  // unset polling status
  anarch::ScopedLock scope(lock);
  polling = false;
}

}
