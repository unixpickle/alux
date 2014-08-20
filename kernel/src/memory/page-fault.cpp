#include "page-fault.hpp"
#include <anarch/api/panic>

namespace Alux {

void HandlePageFault(VirtAddr, bool) {
  // TODO: check current task
  anarch::Panic("Alux::HandlePageFault()");
}

}