#ifndef __ALUX_PORT_HPP__
#define __ALUX_PORT_HPP__

#include "message.hpp"
#include "terminal.hpp"

namespace Alux {

/**
 * A port is an abstract entity which can receive and emit information.
 *
 * Information is stored in the form of a [Message]. A port does not queue
 * messages: if you write mulitple messages to a port in rapid succession, only
 * the last message will arrive at its destination.
 *
 * A port can be connected to and severed from a [Terminal].
 */
class Port {
public:
  /**
   * Call this on a port to send data to its remote end. This will most likely
   * trigger [SendToThis] on a remote port.
   * @critical
   */
  void SendToRemote(const Message &);
  
protected:
  friend class Terminal;
  
  /**
   * Send a message to this port. This will be called by a remote entity to
   * signal this port that data is being sent to it.
   * @critical
   */
  virtual void SendToThis(const Message &) = 0;
};

}

#endif
