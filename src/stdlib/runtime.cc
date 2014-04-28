#include "runtime.h"
#include <platform/failure.h>

void * operator new(size_t, void * p) {
  return p;
}

void * operator new[](size_t, void * p) {
  return p;
}

void * operator new(size_t) {
  return NULL;
}

void * operator new[](size_t) {
  
}

void operator delete(void *) {
}

void operator delete[](void *) {
}

void operator delete(void *, void *) {
  
}

void operator delete[](void *, void *) {
  
}

extern "C" {

void * __dso_handle = 0;

int __cxa_atexit(void (* destructor) (void *), void * arg, void * dso) {
  (void)destructor;
  (void)arg;
  (void)dso;
  return 0;
}

void __cxa_finalize(void * f) {
  (void)f;
}

void __cxa_pure_virtual() {
  OS::Panic("__cxa_pure_virtual() has been called");
}

}
