#include "page-fault.hpp"
#include "../tasks/hold-scope.hpp"
#include "../tasks/user-task.hpp"
#include <anarch/api/panic>

#include <anarch/stream> // TODO: delete this

namespace Alux {

void HandlePageFault(VirtAddr addr, bool write) {
  HoldScope scope;
  if (scope.GetTask().IsUserTask()) {
    UserTask & task = static_cast<UserTask &>(scope.GetTask());
    if (task.GetExecutableMap().HandlePageFault(addr, write)) {
      return;
    }
  }
  anarch::cerr << "unhandled fault " << addr << anarch::endl;
  anarch::Panic("TODO: kill task on unhandled page fault");
}

}