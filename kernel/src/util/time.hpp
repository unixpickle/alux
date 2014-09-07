#ifndef __ALUX_UTIL_TIME_HPP__
#define __ALUX_UTIL_TIME_HPP__

#include <anarch/stdint>

namespace Alux {

/**
 * Returns the time on the global clock (in ticks) that represents the time a
 * certain number of nanoseconds from now.
 */
uint64_t NanosFromNow(uint64_t nanos);

}

#endif
