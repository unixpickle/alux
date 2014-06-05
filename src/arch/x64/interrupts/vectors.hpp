#ifndef __X64_VECTORS_HPP__
#define __X64_VECTORS_HPP__

namespace OS {

namespace x64 {

namespace IntVectors {

const uint8_t PIT = 0x20;
const uint8_t Calibrate = 0x80;
const uint8_t Panic = 0xef;
const uint8_t Invlpg = 0xee;

};

}

}

#endif
