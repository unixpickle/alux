#include <cstddef>

void * operator new(size_t, void * p);
void * operator new[](size_t, void * p);
void * operator new(size_t);
void * operator new[](size_t);
void operator delete(void * ptr);
void operator delete[](void * ptr);
void operator delete(void * ptr , void * place);
void operator delete[](void * ptr, void * place);

extern "C" {

extern void * __dso_handle;
 
int __cxa_atexit(void (* destructor) (void *), void * arg, void * dso);
void __cxa_finalize(void * f);
void __cxa_pure_virtual();

}
