#ifndef __PLATFORM_X64_INT_VECTORS_HPP__
#define __PLATFORM_X64_INT_VECTORS_HPP__

#include <cstdint>

namespace OS {
namespace x64 {

namespace IntVectors {

const uint8_t PIT = 0x20;
const uint8_t Calibrate = 0x80;
const uint8_t Panic = 0xef;

};

}
}

#endif

