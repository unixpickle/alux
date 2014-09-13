#ifndef __ALUX_PORT_HPP__
#define __ALUX_PORT_HPP__

#include "terminal.hpp"

namespace Alux {

/**
 * A port is an abstract entity which can receive and emit information.
 *
 * Information is stored in the form of a [Message]. A port does not queue
 * messages: if you write mulitple messages to a port in rapid succession, only
 * the last message is guaranteed to arrive at its destination.
 *
 * A port can be connected to and severed from a [Terminal].
 */
class Port {
public:
  /**
   * Your subclass might want a fancy destructor. The criticality of the
   * destructor may vary depending on the subclass.
   */
  virtual ~Port() {}
  
  /**
   * Set the terminal of this port if the port does not already have one.
   * @critical
   */
  bool SetTerminal(Terminal &);
  
  /**
   * Call this on a port to send data to its remote end. This will most likely
   * trigger [SendToThis] on a remote port.
   * @critical
   */
  void SendToRemote(const Message &);
  
  /**
   * If this port has a terminal, sever the terminal.
   * @critical
   */
  void Sever();
  
protected:
  friend class Terminal;
  
  /**
   * Send a message to this port. This will be called by a remote entity to
   * signal this port that data is being sent to it.
   * @critical
   */
  virtual void SendToThis(const Message &) = 0;
  
private:
  anarch::CriticalLock lock;
  Terminal * terminal = NULL;
  
  Terminal * GetTerminal(bool sever = false);
};

}

#endif
