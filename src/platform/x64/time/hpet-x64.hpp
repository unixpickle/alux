#include "../acpi/hpet-desc-x64.hpp"
#include <platform/time.hpp>
#include <new>

namespace OS {
namespace x64 {

class HPET : public Clock {
public:
  HPET();

  static bool IsSupported();
  static void Initialize();
  static HPET & GetGlobal();

  virtual uint64_t GetTime();
  virtual uint64_t GetTicksPerMin();
};

}
}
