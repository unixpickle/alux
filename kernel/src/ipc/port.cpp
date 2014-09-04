#include "port.hpp"

namespace Alux {

Endpoint * Port::Open() {
  anarch::ScopedLock scope(endpointLock);
  if (endpoint) return NULL;
  return endpoint = Endpoint::New(*this);
}

void Port::Sever() {
  anarch::ScopedLock scope(endpointLock);
  if (!endpoint) return;
  endpoint->Sever();
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