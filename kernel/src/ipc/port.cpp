#include "port.hpp"
#include <anarch/assert>

namespace Alux {

Endpoint * Port::Open() {
  anarch::ScopedLock scope(endpointLock);
  if (endpoint) return NULL;
  return endpoint = Endpoint::New(*this);
}

void Port::Sever(int reason) {
  anarch::ScopedLock scope(endpointLock);
  if (!endpoint) return;
  bool result = endpoint->Retain();
  assert(result);
  (void)result;
  endpoint->Sever(reason);
  endpoint->Release();
  endpoint = NULL;
}

Endpoint * Port::GetEndpoint() {
  anarch::ScopedLock scope(endpointLock);
  if (!endpoint) return NULL;
  bool result = endpoint->Retain();
  assert(result);
  (void)result;
  return endpoint;
}

}