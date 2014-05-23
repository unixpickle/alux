#include <platform/time.hpp>
#include <platform/memory.hpp>
#include <new>
#include "../acpi/hpet-desc-x64.hpp"

namespace OS {
namespace x64 {

class HPET : public Clock {
private:
  volatile uint64_t * hpetVirtual;
  uint64_t clockSpeed;
  
public:
  HPET();
  ~HPET() {
    Panic("singleton cannot be destroyed");
  }

  static bool IsSupported();
  static void Initialize();
  static HPET & GetGlobal();

  virtual uint64_t GetTime();
  virtual uint64_t GetTicksPerMin();

  void Disable();
  void Enable();
};

}
}
