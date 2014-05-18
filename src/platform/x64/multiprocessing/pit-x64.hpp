#ifndef __PLATFORM_X64_PIT_X64_HPP__
#define __PLATFORM_X64_PIT_X64_HPP__

#include "../interrupts/lapic-x64.hpp"

namespace OS {

namespace x64 {

/**
 * Suggest value 11932 for ~100 Hz
 */
void PitSetDivisor(uint16_t div);

/**
 * Call to increment the PIT counter.
 */
void PitInterruptHandler();

/**
 * Sleep on the PIT for a given number of ticks.
 */
void PitSleep(uint64_t ticks);

}

}

#endif
