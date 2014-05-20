#include "time-init-x64.hpp"

namespace OS {
namespace x64 {

void InitializeTime() {
  CalibrateLapicTimers();

  Panic("TODO: here, setup a better time keeping mechanism.");
  if (SupportsInvariantTSC()) {
    // configure the TSC
  } else if (SupportsHPET()) {
    // look for HPET
  } else {
    // resort to using the PIT
  }
}

bool SupportsInvariantTSC() {
  uint32_t edx;
  CPUID(0x80000007, NULL, &edx, NULL);
  return (edx & (1 << 8)) != 0;
}

bool SupportsHPET() {
  return false;
}

void CalibrateLapicTimers() {
  cout << "OS::x64::CalibrateLapicTimers()" << endl;
  SetIntRoutine(IntVectors::Calibrate, CpuCalibrateLapic);

  LAPIC & lapic = GetLocalAPIC();
  for (int i = 0; i < CPUList::GetCount(); i++) {
    CPU & cpu = CPUList::GetEntry(i);
    if (cpu.apicId != lapic.GetId()) {
      lapic.SendIPI(cpu.apicId, IntVectors::Calibrate, 0, 1, 0);
    } else {
      CpuCalibrateLapic();
    }
  }
  
  bool allDone;
  do {
    allDone = true;
    for (int i = 0; i < CPUList::GetCount(); i++) {
      CPU & cpu = CPUList::GetEntry(i);
      if (!cpu.timeInfo.lapicCalibrated) {
        allDone = false;
        break;
      }
    }
  } while (!allDone);
  
  UnsetIntRoutine(IntVectors::Calibrate);
}

void CpuCalibrateLapic() {
  LAPIC & lapic = GetLocalAPIC();
  PitSleep(1);
  lapic.WriteRegister(LAPIC::RegLVT_TMR, 0xff);
  lapic.WriteRegister(LAPIC::RegTMRINITCNT, 0xffffffff);
  PitSleep(50);

  uint64_t value = lapic.ReadRegister(LAPIC::RegTMRCURRCNT);
  lapic.WriteRegister(LAPIC::RegLVT_TMR, 0x10000);

  CPU & cpu = CPUList::GetCurrentCPU();
  cpu.timeInfo.lapicClock = (uint64_t)(0xffffffff - value) * 2;
  cpu.timeInfo.lapicCalibrated = true;
}

}
}
