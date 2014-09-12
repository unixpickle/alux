#include "terminal.hpp"
#include <anarch/critical>

namespace Alux {

Terminal & Terminal::New(Port * p, GarbageCollector & c) {
  AssertNoncritical();
  Terminal * t = new Terminal(p, c);
  assert(t != NULL);
  return *t;
}

void Terminal::Connect(Terminal & t1, Terminal & t2) {
  AssertNoncritical();
  Connection & conn = Connection::New(t1, t2);
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

}
