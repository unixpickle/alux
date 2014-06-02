#ifndef __MEMORY_SLAB_HPP__
#define __MEMORY_SLAB_HPP__

#include <cstddef>
#include <new>

namespace OS {

class Slab {
public:
  Slab(size_t size); // @noncritical
  void * AllocBuf(); // @noncritical
  void FreeBuf(void *); // @noncritical
  
protected:
  size_t size;
};

template <class T>
class TypedSlab : public Slab {
public:
  typedef Slab super;
  
  /**
   * @noncritical
   */
  TypedSlab() : super(sizeof(T)) {}
  
  /**
   * @noncritical
   */
  template <typename... Args>
  T * New(Args... args) {
    return new(AllocBuf()) T(args...);
  }
  
  /**
   * @noncritical
   */
  void Delete(T * x) {
    x->~T();
    FreeBuf((void *)x);
  }
};

}

#endif
