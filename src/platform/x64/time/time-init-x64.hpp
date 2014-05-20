#ifndef __PLATFORM_X64_TIME_INIT_X64_HPP__
#define __PLATFORM_X64_TIME_INIT_X64_HPP__

#include "../interrupts/lapic-x64.hpp"
#include "../interrupts/int-handlers-x64.hpp"
#include "../interrupts/int-vectors-x64.hpp"
#include "../multiprocessing/cpu-x64.hpp"
#include "../common-x64.hpp"
#include "pit-x64.hpp"

namespace OS {
namespace x64 {

/**
 * By this point, the CPUs should already be initialized.
 */
void InitializeTime();

bool SupportsInvariantTSC();
bool SupportsHPET();

void CalibrateLapicTimers();
void CpuCalibrateLapic();

}
}

#endif

