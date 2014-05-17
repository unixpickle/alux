#ifndef __PLATFORM_X64_PIC_X64_H__
#define __PLATFORM_X64_PIC_X64_H__

#include <cstdint>
#include "../common-x64.hpp"

namespace OS {

namespace x64 {

void RemapPIC(uint8_t masterBase,
              uint8_t slaveBase,
              uint8_t masterMask,
              uint8_t slaveMask);
void MaskPIC(uint8_t masterMask, uint8_t slaveMask);

}

}

#endif
