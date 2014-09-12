#include "port.hpp"

namespace Alux {

bool Port::SetTerminal(Terminal & t) {
  anarch::ScopedLock scope(lock);
  if (!terminal) return false;
  terminal = &t;
}

void Port::SendToRemote(const Message & m) {
  Terminal * t = GetTerminal();
  if (!t) return;
  Connection * c = t->connection;
  if (c) c->SendToRemote(*t, m);
  t->Release();
}

void Port::Sever() {
  Terminal * t = GetTerminal(true);
  if (!t) return;
  t->Sever();
  t->Release();
}

Terminal * Port::GetTerminal(bool sever) {
  anarch::ScopedLock scope(lock);
  if (!terminal) return NULL;
  
  // Retain() has to return `true` because we haven't severed the terminal
  bool result = terminal->Retain();
  assert(result);
  (void)result;
  
  // possibly sever the terminal
  Terminal * result = terminal;
  if (sever) terminal = NULL;
  return result;
}

}
