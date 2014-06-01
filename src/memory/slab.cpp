#include <memory/slab.hpp>
#include <cstdint>
#include <new>

// TODO: implement some kind of super duper efficient linked list!

namespace OS {

Slab::Slab(size_t _size) : size(_size) {}

void * Slab::AllocBuf() {
  return (void *)(new uint8_t[size]);
}

void Slab::FreeBuf(void * x) {
  delete (uint8_t *)x;
}

}
