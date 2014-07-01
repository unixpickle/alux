#include <arch/x64/acpi/hpet.hpp>
#include <arch/x64/acpi/sdt.hpp>
#include <memory/physcopy.hpp>

namespace OS {

namespace x64 {

static HPETInfo info;
static HPETInfo * infoPtr = NULL;
static bool looked = false;

HPETInfo * HPETInfo::GetGlobal() {
  if (looked) return infoPtr;
  looked = true;
  
  SDT & sdt = SDT::GetGlobal();
  int idx = sdt.FindTable("HPET");
  if (idx < 0) return NULL;
  
  PhysCopy(&info, sdt.GetTable(idx), sizeof(info));
  return (infoPtr = &info);
}

}

}
