#ifndef __ALUX_ENDPOINT_HPP__
#define __ALUX_ENDPOINT_HPP__

#include "data.h"
#include <anarch/lock>

namespace Alux {

class Port;

class Endpoint {
public:
  /**
   * Create a new endpoint given a certain port. The returned endpoint will
   * have a retain count of 1.
   * @noncritical
   */
  Endpoint(Port &);
  
  bool Retain(); // @noncritical
  void Release(); // @noncritical
  void Close(int reason); // @noncritical
  
  bool IsConnected(); // @noncritical
  Endpoint * GetRemote(); // @noncritical
  void SetRemote(Endpoint *); // @noncritical
  
  void SetData(const Data &); // @noncritical
  Data GetData(); // @noncritical
  
  int GetControl(); // @noncritical
  void SetControl(int); // @noncritical
  
  void Signal(); // @noncritical
  bool IsSignaled(); // @noncritical
  void Unsignal(); // @noncritical
  
protected:
  anarch::NoncriticalLock portLock;
  Port & port;
  
  anarch::NoncriticalLock remoteLock;
  Endpoint * remote = NULL;
  
  anarch::NoncriticalLock lifeLock;
  int closeReason = 0;
  bool closed = false;
  int retainCount = 1;
};

}

#endif
