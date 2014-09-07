#ifndef __ALUX_POLL_STATE_HPP__
#define __ALUX_POLL_STATE_HPP__

#include <anarch/lock>

namespace Alux {

class Thread;
class UserPort;

/**
 * The part of a thread's state that indicates if it's currently polling on any
 * ports.
 */
class PollState {
public:
  PollState(Thread &);
  
  void Signal(UserPort &); // @ambicritical
  
  static void Poll(); // @critical
  static void Poll(UserPort &); // @critical
  static void Poll(uint64_t nanoseconds); // @critical
  static void Poll(UserPort &, uint64_t nanoseconds); // @critical
  
private:
  Thread & thread;
  anarch::CriticalLock lock;
  bool polling = false;
  UserPort * specific = NULL;
  
  void PollSpecific(UserPort *, uint64_t * nanoseconds);
};

}

#endif
