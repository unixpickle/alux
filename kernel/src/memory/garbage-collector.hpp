#ifndef __ALUX_GARBAGE_COLLECTOR_HPP__
#define __ALUX_GARBAGE_COLLECTOR_HPP__

#include "garbage-object.hpp"
#include <anarch/lock>

namespace OS {

class Thread;

/**
 * A garbage collector is an asynchronous pool used to destroy objects from
 * critical sections.
 */
class GarbageCollector {
public:
  /**
   * Create a new instance, letting it know that it will be running inside a
   * particular scheduled thread. It uses this thread reference to wake itself
   * up when a new piece of garbage is pushed.
   */
  GarbageCollector(Thread & th);
  
  /**
   * The main garbage collection method. This will loop forever with pauses.
   * @noncritical
   */
  void Main();
  
protected:
  /**
   * Push a new piece of garbage to the thread. A garbage object should always
   * push itself through this method.
   * @ambicritical
   */
  void Push(GarbageObject & obj);
  
  friend class GarbageObject;
  
private:
  Thread & thread;
  ansa::CriticalLock lock;
  ansa::LinkedList<GarbageObject> objects;
};

}

#endif
