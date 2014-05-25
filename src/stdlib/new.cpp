#include <new>

void * operator new(size_t, void * p) {
  return p;
}

void * operator new[](size_t, void * p) {
  return p;
}

void * operator new(size_t s) {
  return (void *)1;
}

void * operator new[](size_t s) {
  return (void *)1;
}

void operator delete(void * p) {
}

void operator delete[](void * p) {
}

void operator delete(void *, void *) {
}

void operator delete[](void *, void *) {
}
