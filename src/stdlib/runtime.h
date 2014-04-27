#include <cstddef>

void * operator new(size_t, void * p) throw();
void * operator new[](size_t, void * p) throw();

extern "C" {

extern void * __dso_handle;
 
int __cxa_atexit(void (* destructor) (void *), void * arg, void * dso);
void __cxa_finalize(void * f);
void __cxa_pure_virtual();

}
