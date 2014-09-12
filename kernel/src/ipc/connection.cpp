#include "terminal.hpp" // no need for "connection.hpp"
#include <anarch/critical>

namespace Alux {

void Connection::Connect(Terminal & t1, Terminal & t2) {
  AssertNoncritical();
  Connection * c = new Connection(t1, t2);
  assert(c != NULL);
  
  c->lock.Seize();
  t1.connection = t2.connection = c;
  t1.Deliver(Message::Opened());
  t2.Deliver(Message::Opened());
  c->lock.Release();
}

void Connection::SendToRemote(Terminal & sender, const Message & m) {
  AssertCritical();
  Terminal * other = NULL;
  {
    anarch::ScopedLock scope(lock);
    if (&sender == terminal1) {
      other = terminal2;
    } else {
      assert(&sender == terminal2);
      other = terminal1;
    }
    if (!other) return;
    if (!other->Retain()) return;
  }
  other->Deliver(m);
  other->Release();
}

void Connection::Close(Terminal & sender) {
  AssertNoncritical();
  anarch::ScopedCritical critical;
  Terminal * other = NULL;
  {
    anarch::ScopedLock scope(lock);
    if (&sender == terminal1) {
      other = terminal2;
      terminal1 = NULL;
    } else {
      assert(&sender == terminal2);
      other = terminal1;
      terminal2 = NULL;
    }
    if (!other) {
      anarch::SetCritical(false);
      delete this;
      return;
    }
    if (!other->Retain()) {
      return;
    }
  }
  other->Deliver(Message::Closed());
  other->Release();
}

Connection::Connection(Terminal & x, Terminal & y)
  : terminal1(&x), terminal2(&y) {
}

}
