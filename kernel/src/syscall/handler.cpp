#include "handler.hpp"
#include "console.hpp"
#include <anarch/stream>
#include <anarch/critical>

namespace Alux {

void StandardSyscallHandler(uint16_t number, anarch::SyscallArgs & args) {
  AssertCritical();
  switch (number) {
    case 0:
      PrintSyscall(args);
      break;
    default:
      anarch::cerr << "unknown StandardSyscallHandler(" << number << ", ...)"
        << anarch::endl;
      break;
  }
}

}