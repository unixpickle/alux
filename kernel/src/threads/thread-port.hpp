#ifndef __ALUX_THREAD_PORT_HPP__
#define __ALUX_THREAD_PORT_HPP__

#include "../ipc/port.hpp"
#include <anidmap/id-object>
#include <anidmap/maps>

namespace Alux {

class Thread;

class ThreadPort : public anidmap::IdObject, public Port {
public:
  /**
   * Allocate and initialize a new [ThreadPort]. If the allocation fails, the
   * kernel will Panic().
   * @noncritical
   */
  static ThreadPort & New(Thread &);
  
  /**
   * Attempt to add this port to its thread. If the thread has no available
   * port identifiers, this will fail and return `false`.
   */
  bool AddToThread();
  
  /**
   * Deallocate this port and remove it from its thread if [remove] is `true`.
   */
  void Dealloc(bool remove);
  
protected:
  template <class T, int C>
  friend class anidmap::HashMap;
  ansa::LinkedList<ThreadPort>::Link hashMapLink;
  
  /**
   * Add this port to the thread's polling list and wake up the thread if it
   * is polling.
   * @critical
   */
  virtual void SendToThis(const Message &);
  
private:
  ThreadPort(Thread &);
  Thread & thread;
};

}

#endif
