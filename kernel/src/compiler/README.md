# What is this?

These are a set of functions that you should never call yourself. They exist for the simple purpose of pleasing the build system.

## &lt;cstring&gt; functions

Compilers like Clang and GCC must sometimes insert calls to memset() and memcpy(). In Alux, these functions are not used (in favor of ansa::Memset() and ansa::Memcpy()). Because of this, I had to create simple implementations of the memset() and memcpy() functions that wrap the *ansa* calls. Ultimately, this may be slightly detrimental for performance, but *screw that*.

## C++ stuff

The main annoying symbol that C++ compilers rely on is `__cxa_pure_virtual`. This is the address set in an object's vtable for pure virtual functions. It is generally impossible to call `__cxa_pure_virtual()` indirectly, but it's still needed for code to link.
