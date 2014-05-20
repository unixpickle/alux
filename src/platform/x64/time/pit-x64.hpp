#ifndef __PLATFORM_X64_PIT_X64_HPP__
#define __PLATFORM_X64_PIT_X64_HPP__

#include "../interrupts/lapic-x64.hpp"
#include "time-structures-x64.hpp"

namespace OS {

namespace x64 {

/**
 * Suggest value 11932 for ~100 Hz
 */
void PitSetDivisor(uint16_t div);

/**
 * Returns the number of ticks since the last call to PitSetDivisor()
 */
uint64_t PitGetTicks();

/**
 * Call to increment the PIT counter.
 */
void PitInterruptHandler();

/**
 * Sleep on the PIT for a given number of ticks.
 */
void PitSleep(uint64_t ticks);

/**
 * Wait until the PIT tick count reaches a value.
 */
void PitWait(uint64_t date);

}

}

#endif
