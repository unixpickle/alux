#include "cplusplus.hpp"
#include <anarch/api/panic>

extern "C" {

void * __dso_handle = 0;

int __cxa_atexit(void (*) (void *), void *, void *) {
  return 0;
}

void __cxa_finalize(void *) {
}

void __cxa_pure_virtual() {
  anarch::Panic("__cxa_pure_virtual() - method unimplemented");
}

}
