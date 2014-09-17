#include "../scheduler/scheduler.hpp" // no need for "thread-port.hpp"

namespace Alux {

ThreadPort & ThreadPort::New(Thread & t) {
  ThreadPort * res = new ThreadPort(t);
  assert(res != NULL);
  return *res;
}

bool ThreadPort::AddToThread() {
  return thread.GetPortList().Add(*this);
}

void ThreadPort::Dealloc(bool remove) {
  if (remove) {
    thread.GetPortList().Remove(*this);
    thread.pollState.RemovePending(*this);
  }
  delete this;
}

void ThreadPort::SendToThis(const Message & msg) {
  lastMessage = msg;
  thread.pollState.AddToPending(*this);
}

ThreadPort::ThreadPort(Thread & t)
  : hashMapLink(*this), pollStateLink(*this), thread(t) {
}

}
