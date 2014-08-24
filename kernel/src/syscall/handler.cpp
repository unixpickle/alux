#include "handler.hpp"
#include <anarch/stream>
#include <anarch/critical>

namespace Alux {

void StandardSyscallHandler(uint16_t number, anarch::SyscallArgs &) {
  AssertCritical();
  anarch::cout << "in StandardSyscallHandler(" << number << ", ...)"
    << anarch::endl;
}

}