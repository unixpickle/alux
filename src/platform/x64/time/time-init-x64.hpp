#ifndef __PLATFORM_X64_TIME_INIT_X64_HPP__
#define __PLATFORM_X64_TIME_INIT_X64_HPP__

#include "tsc-x64.hpp"
#include "hpet-x64.hpp"
#include "time-x64.hpp"

namespace OS {
namespace x64 {

/**
 * By this point, the CPUs should already be initialized.
 */
void InitializeTime();

void CalibrateLapicTimers();
void CpuCalibrateLapic();

}
}

#endif

