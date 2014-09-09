#ifndef __ALUX_TERMINAL_HPP__
#define __ALUX_TERMINAL_HPP__

#include <anarch/lock>
#include "../scheduler/garbage-object.hpp"

namespace Alux {

class Port;

/**
 * A terminal is the means by which two ports communicate.
 * 
 * A terminal can be in one of six states: waiting, opened, closed
 * severed-waiting, severed-open, and severed-closed. When a terminal is first
 * created, it is naturally "waiting". Later, a connection may be made between
 * two "waiting" terminals, changing them to the "opened" state.
 *
 * The terminal shutdown process makes the entire concept of a "terminal"
 * complicated to the point of insanity. Imagine that a terminal is a cellular
 * phone and a port is a person. The communications stream between two people
 * talking on phones looks identical to how ports communicate:
 *
 *     person A <-> phone A      <-> phone B      <-> person B
 *     (port A) <-> (terminal A) <-> (terminal B) <-> (port B)
 *
 * However, in real life, a person usually says "goodbye" and then hangs up
 * their phone. The person can then make new calls on their phone and talk to
 * new people. This is NOT how terminals work.
 *
 * Imagine instead that when "person A" wishes to stop communicating with
 * "person B", he throws his phone on the ground and runs away. If "person A"
 * wants to call someone else, or even call back "person B", he must buy a new
 * phone.
 * 
 * Likewise, when the entity that "port A" represents wishes to kill its
 * communication with "port B", "port A" severs its connection with its
 * terminal. "Terminal A" is then responsible for telling "terminal B" that it
 * has been severed. This way, if "terminal B" attempts to send any messages to
 * "terminal A", "port A" will never get them. This prevents race conditions
 * and simplifies the process of port communication.
 */
class Terminal : public GarbageObject {
public:
  Terminal();
  virtual ~Terminal() {}
  
  /**
   * Tell the remote that we have been severed. This sets the remote to a
   * "closed" or "closed-severed" state.
   * @noncritical
   */
  virtual void Dealloc();
  
private:
  friend class Port;
  anarch::CriticalLock portLock;
  Port * port = NULL;
  
  anarch::CriticalLock remoteLock;
  Terminal * remote = NULL;
  bool closed = false;
};

}

#endif
