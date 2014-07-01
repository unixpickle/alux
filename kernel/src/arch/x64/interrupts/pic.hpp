#ifndef __X64_PIC_HPP__
#define __X64_PIC_HPP__

#include <cstdint>

namespace OS {

namespace x64 {

void PICRemap(uint8_t masterBase,
              uint8_t slaveBase,
              uint8_t masterMask,
              uint8_t slaveMask);

}

}

#endif
