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
  assert(retainCount >= 0);
  bool shouldDie = (!retainCount && !port);
  portLock.Release();
  
  // compiler memory barrier because `this` could have been freed
  __asm__("" : : : "memory");
  
  // if we should die, this thread is the last thread with a pointer to `this`.
  if (shouldDie) {
    Endpoint * rem = GetRemote();
    if (rem) {
      rem->RemoteDestroyed(closeReason);
      rem->Release();
    }
    Delete();
  }
}

Endpoint * Endpoint::GetRemote() {
  AssertNoncritical();
  anarch::ScopedLock scope(remoteLock);
  if (!remote) return NULL;
  if (!remote->Retain()) return NULL;
  return remote;
}

void Endpoint::Connect(Endpoint & ep) {
  // this is the only time two remoteLock's will be locked simultaneously, so
  // it is safe
  remoteLock.Seize();
  assert(!remote);
  ep.SetRemote(*this);
  remote = &ep;
  remoteLock.Release();
  
  // signal both endpoints so that they know they're connected
  Send(Data::Empty());
  ep.Send(Data::Empty());
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

void Endpoint::Delete() {
  delete this;
}

Endpoint * Endpoint::New(Port & port) {
  return new Endpoint(port);
}

void Endpoint::Sever(int reason) {
  anarch::ScopedLock scope(portLock);
  if (!port) return;
  closeReason = reason;
  port = NULL;
  assert(retainCount > 0);
}

void Endpoint::RemoteDestroyed(int status) {
  remoteLock.Seize();
  remote = NULL;
  remoteLock.Release();
  Send(Data::Integer(status, false));
}

void Endpoint::SetRemote(Endpoint & ep) {
  anarch::ScopedLock inner(remoteLock);
  assert(!remote);
  remote = &ep;
}

}
