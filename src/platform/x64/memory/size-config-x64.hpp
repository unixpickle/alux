#ifndef __PLATFORM_X64_SIZE_CONFIG_X64_H__
#define __PLATFORM_X64_SIZE_CONFIG_X64_H__

#include <cstddef>

namespace OS {

namespace x64 {

const int ScratchPTCount = 1;
size_t KernelDataSize();
void * ScratchPTStart();

}

}

#endif

