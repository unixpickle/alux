#ifndef __ALUX_GARBAGE_OBJECT_HPP__
#define __ALUX_GARBAGE_OBJECT_HPP__

#include <anarch/stddef>

namespace OS {

class GarbageCollector;

class GarbageObject {
public:
  /**
   * Create a garbage object.
   * @ambicritical
   */
  GarbageObject(GarbageCollector & c);
  
  /**
   * Add this object to the garbage queue to have its Destroy() method called.
   * @critical
   */
  void ThrowAway();
  
  /**
   * Destroy this garbage object immediately and notify the garbage thread
   * about its destruction.
   * @noncritical
   */
  void ThrowAwaySync();
  
protected:
  virtual void Destroy() = 0; // @noncritical
  
  GarbageCollector & garbageCollector;
  GarbageObject * nextGarbage = NULL;
  GarbageObject * lastGarbage = NULL;
  
  friend class GarbageThread;
};

}

#endif
