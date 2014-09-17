#ifndef __ALUX_POLL_STATE_HPP__
#define __ALUX_POLL_STATE_HPP__

#include "thread-port.hpp"

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
  
  friend class ThreadPort;
  void AddToPending(ThreadPort &);
  void RemovePending(ThreadPort &);
  
private:
  anarch::CriticalLock lock;
  bool polling = false;
  Thread & thread;
  ansa::LinkedList<ThreadPort> pendingPorts;
};

}

#endif
