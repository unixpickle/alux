#include "failure-x64.hpp"

namespace OS {

  static bool hasPaniced = false;
  static bool failureInitialized = false;

  namespace x64 {
    
    void InitializeFailure() {
      failureInitialized = true;
      IRT::GetGlobal()[IntVectors::Panic] = FailureIntHandler;
    }
    
    void FailureIntHandler() {
      __asm__("cli\nhlt");
    }
  
  }

  void Panic(const char * message) {
    if (__sync_fetch_and_or(&hasPaniced, 1)) {
      __asm__("cli\nhlt");
    }
    
    __asm__("cli");
    
    // write the error to the screen
    uint16_t * buf = (uint16_t *)0xb8000;
    int i = 0;
    const char * prefix = "PANIC: ";
    while (*prefix) {
      buf[i++] = *prefix | 0xf200;
      prefix++;
    }
    while (*message) {
      buf[i++] = *message | 0xf900;
      message++;
    }
    buf[i] = 0xf920;
    
    if (failureInitialized) {
      x64::CPUList & list = x64::CPUList::GetGlobal();
      x64::LAPIC & lapic = x64::LAPIC::GetCurrent();
      for (int i = 0; i < list.GetCount(); i++) {
        if (list[i].apicId != lapic.GetId()) {
          lapic.SendIPI(list[i].apicId, x64::IntVectors::Panic);
        }
      }
    }
    __asm__("hlt");
  }

  void ForceHalt() {
    // TODO: here, send a halt interrupt to every CPU and then use ACPI
    // shutdown.
    Panic("FORCED HALT");
  }

}
