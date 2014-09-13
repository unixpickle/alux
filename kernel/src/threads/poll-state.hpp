#ifndef __ALUX_POLL_STATE_HPP__
#define __ALUX_POLL_STATE_HPP__

#include "../ipc/message.hpp"
#include <anarch/lock>
#include <anidmap/identifier>

namespace Alux {

class Thread;

class PollState {
public:
  /**
   * Return the next message that is sent to (or queued on) this thread.
   * @critical
   */
  static void Poll(anidmap::Identifier &, Message &);
  
  /**
   * Like [Poll], but will timeout after [nanos] nanoseconds.
   * @critical
   */
  static bool PollTimeout(anidmap::Identifier &, Message &, uint64_t nanos);
  
protected:
  friend class Thread;
  PollState(Thread &);
  
private:
  anarch::CriticalLock lock;
  Thread & thread;
};

}

#endif
