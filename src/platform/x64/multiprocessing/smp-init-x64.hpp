#ifndef __PLATFORM_X64_SMP_INIT_HPP__
#define __PLATFORM_X64_SMP_INIT_HPP__

#include "../interrupts/int-init-x64.hpp"
#include "../interrupts/int-vectors-x64.hpp"
#include "../memory/kernel-map-x64.hpp"
#include "../gdt-x64.hpp"
#include "pit-x64.hpp"
#include "cpu-x64.hpp"

namespace OS {
namespace x64 {

void SetupCpuList(); // call this first
void StartCpus(); // call this next
void CalibrateCpus(); // then this

void IterateApicIds(void (*)(uint32_t));
void StartCpu(uint32_t lapicId);
void CalibrateCpu(uint32_t lapicId);

void * CopyCpuCode(size_t & size);
void UncopyCpuCode(void * backup, size_t size);

void CpuEntrance();
void CpuMain();
void CpuCalibrate();

}
}

#endif
