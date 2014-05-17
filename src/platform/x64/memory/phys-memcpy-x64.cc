#include "phys-memcpy-x64.hpp"

namespace OS {

namespace x64 {

class CopyFunctor {
public:
  VirtAddr start;
  CopyFunctor(VirtAddr s) : start(s) {}
};

class CopyToFunctor : public CopyFunctor {
public:
  CopyToFunctor(VirtAddr s) : CopyFunctor(s) {}
  void operator()(void * dest, size_t off, size_t size) const {
    memcpy(dest, (void *)(start + off), size);
  }
};

class CopyFromFunctor : public CopyFunctor {
public:
  CopyFromFunctor(VirtAddr s) : CopyFunctor(s) {}
  void operator()(void * source, size_t off, size_t size) const {
    memcpy((void *)(start + off), source, size);
  }
};

void MemcpyToPhys(PhysAddr phys, VirtAddr virt, size_t size) {
  CopyToFunctor fn(virt);
  VirtualizePhysical(phys, size, fn);
}

void MemcpyToVirt(VirtAddr virt, PhysAddr phys, size_t size) {
  CopyFromFunctor fn(virt);
  VirtualizePhysical(phys, size, fn);
}

}

}

