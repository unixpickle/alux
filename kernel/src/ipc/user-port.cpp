#include "user-port.hpp"
#include "../tasks/thread.hpp"
#include <anarch/critical>

namespace Alux {

UserPort * UserPort::ReadNext(Data & dataOut) {
  AssertNoncritical();
  Thread * th = Thread::GetCurrent();
  assert(th != NULL);
  anarch::ScopedLock scope(th->waitingLock);
  UserPort * next = th->waitingPorts.Shift();
  if (!next) return NULL;
  next->signaled = false;
  dataOut = next->data;
  return next;
}

UserPort * UserPort::New(Thread & th) {
  UserPort * result = new UserPort(th);
  if (!result) return NULL;
  if (!result->Init()) {
    delete result;
    return NULL;
  }
  return result;
}

void UserPort::Delete() {
  Delete(true);
}

void UserPort::Delete(bool remove) {
  if (remove) Deinit();
  delete this;
}

bool UserPort::Read(Data & out) {
  if (!thread.Retain()) return false;
  
  thread.waitingLock.Seize();
  if (!signaled) return false;
  thread.waitingPorts.Remove(&waitingLink);
  signaled = false;
  out = data;
  thread.waitingLock.Release();
  
  thread.Release();
  return true;
}

void UserPort::Send(const Data & d) {
  if (!thread.Retain()) return;
  
  thread.waitingLock.Seize();
  data = d;
  if (!signaled) {
    signaled = true;
    thread.waitingPorts.Add(&waitingLink);
    thread.GetPollState().Signal(*this);
  }
  thread.waitingLock.Release();
  
  thread.Release();
}

UserPort::UserPort(Thread & th)
  : hashMapLink(*this), thread(th), waitingLink(*this) {
}

bool UserPort::Init() {
  return thread.GetPorts().Add(*this);
}

void UserPort::Deinit() {
  thread.GetPorts().Remove(*this);
}

}
