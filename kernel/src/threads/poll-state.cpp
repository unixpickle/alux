#include "poll-state.hpp"
#include "thread.hpp"

namespace Alux {

void PollState::Poll(anidmap::Identifier &, Message &) {
  // TODO: this
}

bool PollState::PollTimeout(anidmap::Identifier &, Message &, uint64_t) {
  // TODO: this
}

PollState::PollState(Thread & t) : thread(t) {
}

}
