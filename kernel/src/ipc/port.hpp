#ifndef __ALUX_PORT_HPP__
#define __ALUX_PORT_HPP__

#include "endpoint.hpp"

namespace Alux {

class Port {
public:
  Endpoint * Open(); // @noncritical
  void Close(int reason); // @noncritical
  
  virtual void Signal() = 0; // @noncritical
  virtual bool IsSignaled() = 0; // @noncritical
  virtual void Unsignal() = 0; // @noncritical
  
private:
  anarch::NoncriticalLock endpointLock;
  Endpoint * endpoint;
};

}

#endif
