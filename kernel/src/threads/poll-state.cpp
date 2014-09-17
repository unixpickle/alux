#include "../scheduler/scheduler.hpp"

namespace Alux {

void PollState::Poll(anidmap::Identifier &, Message &) {
  // TODO: this
}

bool PollState::PollTimeout(anidmap::Identifier &, Message &, uint64_t) {
  // TODO: this
}

PollState::PollState(Thread & t) : thread(t) {
}

void PollState::AddToPending(ThreadPort & port) {
  anarch::ScopedLock scope(lock);
  if (port.isQueued) return;
  port.isQueued = true;
  pendingPorts.Add(&port.pollStateLink);
  if (polling) {
    thread.GetTask().GetScheduler().ClearTimeout(thread);
  }
}

void PollState::RemovePending(ThreadPort & port) {
  anarch::ScopedLock scope(lock);
  if (!port.isQueued) return;
  port.isQueued = false;
  pendingPorts.Remove(&port.pollStateLink);
}

}
