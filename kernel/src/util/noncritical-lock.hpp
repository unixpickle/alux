#ifndef __ALUX_ANARCH_NONCRITICAL_LOCK_HPP__
#define __ALUX_ANARCH_NONCRITICAL_LOCK_HPP__

#include <ansa/lock>
#include <ansa/linked-list>

namespace Alux {

class Thread;

class NoncriticalLock : public ansa::Lock {
public:
  virtual void Seize();
  virtual void Release();
  
private:
  struct WaitingThread {
    WaitingThread() : link(*this) {}
    
    Thread * thread = NULL;
    ansa::LinkedList<WaitingThread>::Link link;
    ansa::Atomic<bool> owner;
  };
  
  ansa::OrderedLock queueLock;
  bool held = false;
  ansa::LinkedList<WaitingThread> queue;
};

}

namespace anarch {

using Alux::NoncriticalLock;

}

#endif
