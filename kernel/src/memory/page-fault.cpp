#include "page-fault.hpp"
#include <anarch/api/panic>

namespace OS {

void HandlePageFault(VirtAddr, bool) {
  // TODO: check current task
  anarch::Panic("OS::HandlePageFault()");
}

}