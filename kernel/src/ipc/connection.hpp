#ifndef __ALUX_CONNECTION_HPP__
#define __ALUX_CONNECTION_HPP__

#include <anarch/lock>
#include "message.hpp"

namespace Alux {

class Terminal;

/**
 * A [Connection] allows [Message]s to be passed between [Terminal]s. The
 * connection facilitates synchronization between terminals.
 *
 * If two terminals simply tracked their connection to one another without a
 * neutral [Connection] object, terminal deallocation would be tricky in the
 * case where both terminals are severed simultaneously.
 */
class Connection {
public:
  /**
   * Allocate a new connection given two terminals. Both terminals must be
   * retained. A "connected" message will be sent to both terminals.
   * @noncritical
   */
  static Connection & New(Terminal & t1, Terminal & t2);
  
protected:
  friend class Terminal;
  
  /**
   * Send a message to the terminal that is not [sender].
   * @critical
   */
  void SendToRemote(Terminal & sender, const Message & m);
  
  /**
   * Close the terminal [sender]. If both terminals have been closed, this
   * [Connection] is deallocated. Otherwise, the terminal that is not [sender]
   * is sent a termination message.
   * @noncritical
   */
  void Close(Terminal & sender);
  
private:
  Connection(Terminal & x, Terminal & y);
  
  anarch::CriticalLock lock;
  Terminal * terminal1;
  Terminal * terminal2;
};

}

#endif
