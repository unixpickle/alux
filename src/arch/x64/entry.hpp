#ifndef __X64_ENTRY_HPP__
#define __X64_ENTRY_HPP__

extern "C" {

void MbootEntry(void * mbootPtr);

/**
 * Only used when not compiling on OS X.
 */
void _MbootEntry(void * mbootPtr);

}

#endif
