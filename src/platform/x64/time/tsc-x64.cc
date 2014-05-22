#include "tsc-x64.hpp"

namespace OS {
namespace x64 {

static uint64_t calibrateWaitUntil;
static int32_t calibrateRemaining;
static TSC globalTSC;

static void CpuCalibrateTSC();
static uint64_t RDTSC();

bool TSC::IsSupported() {
  // TODO: check specific CPU families
  uint32_t edx;
  CPUID(0x80000007, NULL, &edx, NULL);
  return (edx & (1 << 8)) != 0;
}

void TSC::Initialize() {
  cout << "OS::x64::TSC::Initialize()" << endl;
  new(&globalTSC) TSC();
  
  // calibrate CPUs
  SetIntRoutine(IntVectors::Calibrate, CpuCalibrateTSC);

  calibrateWaitUntil = PIT::GetGlobal().GetTime() + 50;

  CPUList & cpuList = CPUList::GetGlobal();

  // tell each CPU to do calibrate
  LAPIC & lapic = GetLocalAPIC();
  calibrateRemaining = cpuList.GetCount();
  for (int i = 0; i < cpuList.GetCount(); i++) {
    CPU & cpu = cpuList[i];
    if (cpu.apicId == lapic.GetId()) continue;
    lapic.SendIPI(cpu.apicId, IntVectors::Calibrate, 0, 1, 0);
  }
  
  while (calibrateRemaining);
  UnsetIntRoutine(IntVectors::Calibrate);
  
  // find the lowest offset value
  uint64_t lowestOff = ~(uint64_t)1;
  uint64_t highestOff = 0;
  for (int i = 0; i < cpuList.GetCount(); i++) {
    CPU & cpu = cpuList[i];
    if (cpu.timeInfo.tscOffset < lowestOff) {
      lowestOff = cpu.timeInfo.tscOffset;
    }
    if (cpu.timeInfo.tscOffset > highestOff) {
      highestOff = cpu.timeInfo.tscOffset;
    }
  }
  
  // remove the lowest offset from all of them
  for (int i = 0; i < cpuList.GetCount(); i++) {
    cpuList[i].timeInfo.tscOffset -= lowestOff;
  }
  
  cout << "OS::x64::CalibrateTSC() - biggest diff = "
    << highestOff - lowestOff << endl;
}

TSC & TSC::GetGlobal() {
  return globalTSC;
}

TSC::TSC() {
  // calculate rate
  PIT::GetGlobal().Sleep(1);
  uint64_t start = RDTSC();
  PIT::GetGlobal().Sleep(50);
  uint64_t end = RDTSC();
  ticksPerMin = (end - start) * 120;
}

uint64_t TSC::GetTime() {
  ScopeCritical critical;
  
  CPU & cpu = CPUList::GetGlobal().GetCurrent();
  return RDTSC() - cpu.timeInfo.tscOffset;
}

uint64_t TSC::GetTicksPerMin() {
  return ticksPerMin;
}

static void CpuCalibrateTSC() {
  CPU & cpu = CPUList::GetGlobal().GetCurrent();
  PIT::GetGlobal().WaitUntil(calibrateWaitUntil);
  
  cpu.timeInfo.tscOffset = RDTSC();
  __asm__("lock decl (%0)" : : "r" (&calibrateRemaining));
}

static uint64_t RDTSC() {
  uint32_t eax, edx;
  // TODO: here, use mfence or lfence depending on the CPU (AMD => mfence)
  __asm__("mfence\n"
          "rdtsc" : "=a" (eax), "=d" (edx));
  return (uint64_t)edx << 32 | (uint64_t)eax;
}

}
}
