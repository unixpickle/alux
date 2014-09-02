#ifndef __ALUX_ENDPOINT_HPP__
#define __ALUX_ENDPOINT_HPP__

#include "data.h"
#include <anarch/lock>

namespace Alux {

class Port;
class EndpointQueue;

class Endpoint {
public:
  /**
   * Create a new endpoint given a certain port. The returned endpoint will
   * have a retain count of 1.
   * @noncritical
   */
  Endpoint(Port &);
  
  virtual ~Endpoint(); // @noncritical
  
  // operations involving the lifetime of this endpoint
  bool Retain(); // @noncritical
  void Release(); // @noncritical
  void Close(int reason); // @noncritical
  
  // operations involving the remote end
  bool HasConnected(); // @noncritical
  bool IsConnected(); // @noncritical
  Endpoint * GetRemote(); // @noncritical
  void SetRemote(Endpoint *); // @noncritical
  
  // operations acting on the port
  void SetData(const Data &); // @noncritical  
  int GetControl(); // @noncritical
  void Signal(); // @noncritical
  
private:
  virtual void Delete();
  
  anarch::NoncriticalLock portLock;
  Port * port;
  
  anarch::NoncriticalLock remoteLock;
  bool hasConnected = false;
  Endpoint * remote = NULL;
  
  anarch::NoncriticalLock lifeLock;
  int closeReason = 0;
  bool closed = false;
  int retainCount = 1;
};

}

#endif
