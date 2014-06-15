#include <memory/malloc.hpp>
#include <new>
#include <lock>
#include <macros>
#include <panic>
#include <critical>

static uint64_t lock OS_ALIGNED(8);

void * operator new(size_t, void * p) {
  return p;
}

void * operator new[](size_t, void * p) {
  return p;
}

void * operator new(size_t s) {
  AssertNoncritical();
  OS::ScopeLock scope(&lock);
  OS::Malloc & theMalloc = OS::Malloc::GetGlobal();
  assert(!theMalloc.IsUninitialized());
  void * res = theMalloc.Alloc(s);
  if (!res) OS::Panic("operator new - OOM");
  return res;
}

void * operator new[](size_t s) {
  AssertNoncritical();
  return operator new(s);
}

void operator delete(void * p) {
  AssertNoncritical();
  OS::ScopeLock scope(&lock);
  OS::Malloc & theMalloc = OS::Malloc::GetGlobal();
  assert(!theMalloc.IsUninitialized());
  theMalloc.Free(p);
}

void operator delete[](void * p) {
  AssertNoncritical();
  operator delete(p);
}

void operator delete(void *, void *) {
}

void operator delete[](void *, void *) {
}
