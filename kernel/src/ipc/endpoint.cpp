#include "endpoint.hpp"
#include "port.hpp"
#include <anarch/critical>

namespace Alux {

Endpoint::Endpoint(Port & p) : port(&p) {
  AssertNoncritical();
}

Endpoint::~Endpoint() {
  AssertNoncritical();
}

bool Endpoint::Retain() {
  AssertNoncritical();
  anarch::ScopedLock scope(lifeLock);
  if (closed) return false;
  ++retainCount;
  return true;
}

void Endpoint::Release() {
  AssertNoncritical();
  
  lifeLock.Seize();
  --retainCount;
  bool shouldDie = (!retainCount && closed);
  lifeLock.Release();
  
  if (shouldDie) {
    Endpoint * ep = GetRemote();
    if (ep) {
      ep->SetRemote(NULL);
      ep->Signal();
      ep->Release();
    }
    Delete();
  }
}

void Endpoint::Close(int reason) {
  AssertNoncritical();
  anarch::ScopedLock scope(lifeLock);
  closed = true;
  assert(retainCount > 0);
}

bool Endpoint::HasConnected() {
  AssertNoncritical();
  anarch::ScopedLock scope(remoteLock);
  return hasConnected;
}

bool Endpoint::IsConnected() {
  AssertNoncritical();
  anarch::ScopedLock scope(remoteLock);
  return remote != NULL;
}

Endpoint * Endpoint::GetRemote() {
  AssertNoncritical();
  anarch::ScopedLock scope(remoteLock);
  if (!remote) return NULL;
  if (!remote->Retain()) return NULL;
  return remote;
}

void Endpoint::SetRemote(Endpoint * ep) {
  AssertNoncritical();
  anarch::ScopedLock scope(remoteLock);
  assert(!hasConnected || ep == NULL);
  hasConnected = true;
  remote = ep;
}

void Endpoint::SetData(const Data & data) {
  AssertNoncritical();
  anarch::ScopedLock scope(portLock);
  if (!port) return;
  port->SetData(data);
}

int Endpoint::GetControl() {
  AssertNoncritical();
  anarch::ScopedLock scope(portLock);
  if (!port) return 0;
  
}

void Endpoint::Signal() {
  // TODO: signal the port
}

void Endpoint::Delete() {
  delete this;
}

}
