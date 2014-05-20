#ifndef __PLATFORM_X64_TSC_X64_HPP__
#define __PLATFORM_X64_TSC_X64_HPP__

#include "../interrupts/lapic-x64.hpp"
#include "../interrupts/int-handlers-x64.hpp"
#include "../interrupts/int-vectors-x64.hpp"
#include "../multiprocessing/cpu-x64.hpp"
#include "../common-x64.hpp"
#include "pit-x64.hpp"

namespace OS {
namespace x64 {

bool SupportsInvariantTSC();

void CalibrateTSC();
void CpuCalibrateTSC();

uint64_t GetTSCRate();
uint64_t GetTSCValue();

}
}

#endif
