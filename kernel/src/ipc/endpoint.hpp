#ifndef __ALUX_ENDPOINT_HPP__
#define __ALUX_ENDPOINT_HPP__

#include "data.h"
#include <anarch/lock>

namespace Alux {

class Port;

/**
 * An "endpoint" is the means by which a port connects to another port. Both
 * ports expose their functionality through an endpoint. The endpoint can then
 * be severed from a port at any time so that cleanup is possible while the
 * port is free to create a new endpoint as soon as it wants.
 */
class Endpoint {
public:
  /**
   * If this endpoint has been severed from its port, this returns `false`.
   * Otherwise, it increments the retain counter and returns `true`.
   * @noncritical
   */
  bool Retain();
  
  /**
   * Decrement the retain counter. If the retain counter reaches 0 and this
   * endpoint has been severed, the remote will be notified and the endpoint
   * will be deleted.
   * @noncritical
   */
  void Release();
  
  /**
   * Returns a pointer to the remote endpoint. If this endpoint is not
   * connected, or the remote has been severed, this will return NULL.
   * @noncritical
   */
  Endpoint * GetRemote();
  
  /**
   * Connect this endpoint to a remote. Neither endpoint should already be
   * connected.
   *
   * If you attempt to call Connect() between two sockets simultaneously in the
   * opposite order, a deadlock will probably occur. Generally, Connect should
   * only be called once per pair of endpoints.
   *
   * @noncritical
   */
  void Connect(Endpoint &);
  
  /**
   * Send data to the endpoint.
   * @noncritical
   */
  void Send(const Data &);
  
  /**
   * Get the control flags of this endpoint's owning port.
   * @noncritical
   */
  int GetControl();
  
protected:
  /**
   * Create a new endpoint given a certain port. The resultant endpoint has a
   * retain count of 1.
   * @noncritical
   */
  Endpoint(Port &);
  
  /**
   * You may implement your own fancy destructors.
   * @noncritical
   */
  virtual ~Endpoint() {}
  
  /**
   * Implement this in your subclass for whatever allocation method you choose
   * to use for instances of your subclass.
   * @noncritical
   */
  virtual void Delete();
  
  friend class Port;
  
  static Endpoint * New(Port &);
  
  /**
   * Sever this endpoint from the port that owns it. After this has returned,
   * this endpoint will never act on the owning port again; future Retain()s
   * on this endpoint will fail.
   * @noncritical
   */
  virtual void Sever(int reason);
  
private:  
  anarch::NoncriticalLock remoteLock;
  Endpoint * remote = NULL;
  
  anarch::NoncriticalLock portLock;
  Port * port;
  int retainCount = 1;
  int closeReason = 0;
  
  /**
   * When an endpoint is severed and its retain count reaches zero, this is
   * called.
   * @noncritical
   */
  void RemoteDestroyed(int status);
  
  /**
   * Called on this endpoint when another endpoint attempts to Connect() to us.
   * @noncritical
   */
  bool SetRemote(Endpoint &);
};

}

#endif
