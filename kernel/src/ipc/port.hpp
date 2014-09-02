#ifndef __ALUX_PORT_HPP__
#define __ALUX_PORT_HPP__

#include "endpoint.hpp"

namespace Alux {

class Port {
public:
  Endpoint * Open(); // @noncritical
  void Close(int reason); // @noncritical
  Endpoint * GetEndpoint(); // @noncritical
  
  virtual void Signal() = 0; // @noncritical
  virtual bool IsSignaled() = 0; // @noncritical
  virtual void Unsignal() = 0; // @noncritical
  
  void ReadData(Data & output);
  void WriteData(const Data & input);
  
  int GetControl();
  void SetControl(int);
  
private:
  anarch::NoncriticalLock endpointLock;
  Endpoint * endpoint;
  
  anarch::NoncriticalLock dataLock;
  Data data;
  
  ansa::Atomic<int> control;
};

}

#endif
