#ifndef __ALUX_CPLUSPLUS_HPP__
#define __ALUX_CPLUSPLUS_HPP__

// a little ironic that all these C++ handles aren't in a C++ namespace, don't
// you think?
extern "C" {

extern void * __dso_handle;
 
int __cxa_atexit(void (* destructor) (void *), void * arg, void * dso);
void __cxa_finalize(void * f);
void __cxa_pure_virtual();

}

#endif
