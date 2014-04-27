#include "runtime.h"
#include <platform/failure.h>

void * operator new(size_t, void * p) throw() {
  return p;
}

void * operator new[](size_t, void * p) throw() {
  return p;
}

extern "C" {

void * __dso_handle = 0;

int __cxa_atexit(void (* destructor) (void *), void * arg, void * dso) {
  return 0;
}

void __cxa_finalize(void * f) {
}

void __cxa_pure_virtual() {
  OS::Panic("__cxa_pure_virtual() has been called");
}

}
