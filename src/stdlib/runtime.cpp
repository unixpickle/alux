#include <stdlib/runtime.hpp>
#include <panic>

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
  OS::Panic("__cxa_pure_virtual() - method unimplemented");
}

}
