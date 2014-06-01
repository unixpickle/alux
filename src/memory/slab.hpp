#ifndef __MEMORY_SLAB_HPP__
#define __MEMORY_SLAB_HPP__

#include <cstddef>

namespace OS {

class Slab {
public:
  Slab(size_t size);
  void * AllocBuf();
  void FreeBuf(void *);
  
protected:
  size_t size;
};

template <class T>
class TypedSlab : public Slab {
public:
  typedef Slab super;
  
  TypedSlab() : super(sizeof(T)) {}
  
  template <typename... Args>
  T * New(Args... args) {
    return new(AllocBuf()) T(args...);
  }
  
  void Delete(T * x) {
    x->~T();
    FreeBuf((void *)x);
  }
};

}

#endif
