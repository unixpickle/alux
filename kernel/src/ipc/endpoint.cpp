#include "endpoint.hpp"
#include "port.hpp"
#include <anarch/critical>

namespace Alux {

bool Endpoint::Retain() {
  AssertNoncritical();
  anarch::ScopedLock scope(portLock);
  if (!port) return false;
  ++retainCount;
  return true;
}

void Endpoint::Release() {
  AssertNoncritical();
  
  portLock.Seize();
  --retainCount;
  bool shouldDie = (!retainCount && !port);
  portLock.Release();
  
  // compiler memory barrier because `this` could have been freed
  __asm__("" : : : "memory");
  
  // if we should die, this thread is the last thread with a pointer to `this`.
  if (shouldDie) Delete();
}

Endpoint * Endpoint::GetRemote() {
  AssertNoncritical();
  anarch::ScopedLock scope(remoteLock);
  if (!remote) return NULL;
  if (!remote->Retain()) return NULL;
  return remote;
}

void Endpoint::Send(const Data & data) {
  AssertNoncritical();
  anarch::ScopedLock scope(portLock);
  if (!port) return;
  port->Send(data);
}

int Endpoint::GetControl() {
  AssertNoncritical();
  anarch::ScopedLock scope(portLock);
  if (!port) return 0;
  return port->GetControl();
}

Endpoint::Endpoint(Port & p) : port(&p) {
  AssertNoncritical();
}

void RemoteSevered(int status) {
  remoteLock.Seize();
  remote = NULL;
  remoteLock.Release();
  Send(Data::Integer(status, false));
}

void SetRemote(Endpoint * ep) {
  portLock.Seize();
  
  remoteLock.Seize();
  assert(!remote);
  remote = ep;
  remoteLock.Release();
  
  // notify the port that we got a connection
  Send(Data::Empty());
}

}
