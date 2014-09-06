#ifndef __ALUX_PORT_HPP__
#define __ALUX_PORT_HPP__

#include "endpoint.hpp"

namespace Alux {

class Port {
public:
  /**
   * You may use a destructor in a subclass.
   * @noncritical
   */
  virtual ~Port() {}
  
  /**
   * Open a simple [Endpoint] instance. If this port already has an endpoint,
   * this will return NULL. Otherwise, the returned endpoint will be retained.
   * @noncritical
   */
  Endpoint * Open();
  
  /**
   * Severs the current endpoint if one exists.
   * @noncritical
   */
  void Sever(int reason);
  
  /**
   * Returns a retained reference to the port's current endpoint. If the port
   * has never been connected, or its last endpoint has been severed, this will
   * return NULL.
   * @noncritical
   */
  Endpoint * GetEndpoint();
  
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
