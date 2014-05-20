#include "tsc-x64.hpp"

namespace OS {
namespace x64 {

static uint64_t calibrateWaitUntil;
static int32_t calibrateRemaining;
static uint64_t rate;

bool SupportsInvariantTSC() {
  uint32_t edx;
  CPUID(0x80000007, NULL, &edx, NULL);
  return (edx & (1 << 8)) != 0;
}

void CalibrateTSC() {
  // calibrate CPUs
  cout << "OS::x64::CalibrateTSC()" << endl;
  SetIntRoutine(IntVectors::Calibrate, CpuCalibrateTSC);

  calibrateWaitUntil = PitGetTicks() + 50;

  // tell each CPU to do calibrate
  LAPIC & lapic = GetLocalAPIC();
  calibrateRemaining = CPUList::GetCount();
  for (int i = 0; i < CPUList::GetCount(); i++) {
    CPU & cpu = CPUList::GetEntry(i);
    if (cpu.apicId == lapic.GetId()) continue;
    lapic.SendIPI(cpu.apicId, IntVectors::Calibrate, 0, 1, 0);
  }
  
  while (calibrateRemaining);
  UnsetIntRoutine(IntVectors::Calibrate);
  
  // find the lowest offset value
  uint64_t lowestOff = ~(uint64_t)1;
  uint64_t highestOff = 0;
  for (int i = 0; i < CPUList::GetCount(); i++) {
    CPU & cpu = CPUList::GetEntry(i);
    if (cpu.timeInfo.tscOffset < lowestOff) {
      lowestOff = cpu.timeInfo.tscOffset;
    }
    if (cpu.timeInfo.tscOffset > highestOff) {
      highestOff = cpu.timeInfo.tscOffset;
    }
  }
  
  // remove the lowest offset from all of them
  for (int i = 0; i < CPUList::GetCount(); i++) {
    CPUList::GetEntry(i).timeInfo.tscOffset -= lowestOff;
  }
  
  // calculate rate
  PitSleep(1);
  uint64_t start = GetTSCValue();
  PitSleep(50);
  uint64_t end = GetTSCValue();
  rate = (end - start) * 2;
  
  cout << "OS::x64::CalibrateTSC() - biggest diff = "
    << highestOff - lowestOff << endl;
}

void CpuCalibrateTSC() {
  CPU & cpu = CPUList::GetCurrent();
  PitWait(calibrateWaitUntil);
  
  cpu.timeInfo.tscOffset = GetTSCValue();
  __asm__("lock decl (%0)" : : "r" (&calibrateRemaining));
}

uint64_t GetTSCRate() {
  return rate;
}

uint64_t GetTSCValue() {
  uint32_t eax, edx;
  // TODO: here, use mfence or lfence depending on the CPU
  __asm__("mfence\n"
          "rdtsc" : "=a" (eax), "=d" (edx));
  return (uint64_t)edx << 32 | (uint64_t)eax;
}

}
}
