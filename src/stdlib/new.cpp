#include <new>
#include <arch/general/failure.hpp>
#include <memory/malloc.hpp>
#include <utilities/lock.hpp>
#include <common>

static uint64_t lock OS_ALIGNED(8) = 0;
static bool initialized = false;

void * operator new(size_t, void * p) {
  return p;
}

void * operator new[](size_t, void * p) {
  return p;
}

void * operator new(size_t s) {
  OS::ScopeLock scope(&lock);
  if (!initialized) {
    OS::Malloc::Initialize();
    initialized = true;
  }
  void * res = OS::Malloc::GetGlobal().Alloc(s);
  if (!res) OS::Panic("operator new - OOM");
  return res;
}

void * operator new[](size_t s) {
  return (void *)(new uint8_t[s]);
}

void operator delete(void * p) {
  OS::ScopeLock scope(&lock);
  assert(initialized);
  OS::Malloc::GetGlobal().Free(p);
}

void operator delete[](void * p) {
  delete (uint8_t *)p;
}

void operator delete(void *, void *) {
}

void operator delete[](void *, void *) {
}
