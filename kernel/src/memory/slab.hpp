#ifndef __MEMORY_SLAB_HPP__
#define __MEMORY_SLAB_HPP__

#include <anslabs>
#include <macros>

namespace OS {

template <class T, size_t Count>
class Slab : public ANSlabs::Structor<T, ANSlabs::Cap<Count, sizeof(T)> > {
public:
  typedef ANSlabs::Structor<T, ANSlabs::Cap<Count, sizeof(T)> > super;
  
  template <typename... Args>
  T * New(Args... args);
  
  void Delete(T * x);
  
protected:
  uint64_t lock OS_ALIGNED(8) = 0;
};

}

#include "slab-public.hpp"

#endif
