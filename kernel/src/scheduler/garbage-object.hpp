#ifndef __ALUX_GARBAGE_OBJECT_HPP__
#define __ALUX_GARBAGE_OBJECT_HPP__

#include <ansa/linked-list>

namespace Alux {

class GarbageCollector;

class GarbageObject {
public:
  /**
   * Add this object to the garbage collector.
   * @ambicritical
   */
  void ThrowAway();
  
  /**
   * Deallocate this object on the spot.
   * @noncritical
   */
  virtual void Dealloc() = 0;
  
protected:
  GarbageObject(GarbageCollector &);
  virtual ~GarbageObject();
  
  friend class GarbageCollector;
  ansa::LinkedList<GarbageObject>::Link garbageLink;
  
private:
  GarbageCollector & collector;
};

}

#endif
