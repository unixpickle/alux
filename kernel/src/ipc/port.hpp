#ifndef __ALUX_PORT_HPP__
#define __ALUX_PORT_HPP__

#include "endpoint.hpp"

namespace Alux {

class Port {
public:
  /**
   * Open a simple [Endpoint] instance. If this port already has an endpoint,
   * this will return NULL. Otherwise, the returned endpoint will be retained.
   * @noncritical
   */
  Endpoint * Open() = 0;
  
  /**
   * If this port has an endpoint, it will be severed by calling this method.
   * @noncritical
   */
  void Sever() = 0;
  
  /**
   * Return `true` if this is a [UserPort].
   * @ambicritical
   */
  virtual bool IsUser();
  
  /**
   * Returns a retained reference to the port's current endpoint. If the port
   * has never been connected, or its last endpoint has been severed, this will
   * return NULL.
   * @noncritical
   */
  virtual Endpoint * GetEndpoint();
  
  /**
   * If a piece of data has been signaled on this port, this will return `true`
   * and set [data]. Otherwise, it will return `false`.
   * @noncritical
   */
  virtual bool Receive(Data & data) = 0;
  
  /**
   * Get the control data.
   * @noncritical
   */
  inline int GetControl() {
    return control;
  }
  
  /**
   * Set the control data.
   * @noncritical
   */
  inline void SetControl(int value) {
    control = value;
  }
  
protected:
  friend class Endpoint;
  
  /**
   * Signal this port and write information to it.
   * @noncritical
   */
  virtual void Send(const Data & data) = 0;
  
  anarch::NoncriticalLock endpointLock;
  Endpoint * endpoint = NULL;
  
private:
  ansa::Atomic<int> control;
};

}

#endif
