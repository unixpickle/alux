#include <cstdint>

namespace OS {

/**
 * Returns the current platform-dependent timestap.
 * @discussion This timestamp must never go down and should increase at a
 * constant rate.
 */
uint64_t GetTimestamp();

/**
 * Returns the number of timestamp ticks that will occur in a certain number of
 * seconds. This is done as opposed to returning the frequency in Hz because it
 * allows for more precision without requiring floating point numbers.
 */
uint64_t GetTicksPerTime(int seconds);

}

