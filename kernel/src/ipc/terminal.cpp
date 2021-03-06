#include "port.hpp" // no need to import "terminal.hpp" directly
#include <anarch/critical>

namespace Alux {

Terminal & Terminal::New(Port * p, GarbageCollector & c) {
  AssertNoncritical();
  Terminal * t = new Terminal(p, c);
  assert(t != NULL);
  return *t;
}

Terminal::Terminal(Port * p, GarbageCollector & c)
  : GarbageObject(c), port(p) {
}

bool Terminal::Retain() {
  anarch::ScopedLock scope(portLock);
  if (!port) return false;
  ++retainCount;
  return true;
}

void Terminal::Release() {
  portLock.Seize();
  --retainCount;
  bool closed = (retainCount == 0 && !port);
  portLock.Release();
  if (closed) ThrowAway();
}

void Terminal::Dealloc() {
  AssertNoncritical();
  Hangup();
  delete this;
}

void Terminal::Hangup() {
  AssertNoncritical();
  if (connection) {
    connection->Close(*this);
  }
}

void Terminal::Deliver(const Message & m) {
  anarch::ScopedLock scope(portLock);
  if (!port) return;
  port->SendToThis(m);
}

void Terminal::Sever() {
  anarch::ScopedLock scope(portLock);
  port = NULL;
  assert(retainCount > 0);
}

}
