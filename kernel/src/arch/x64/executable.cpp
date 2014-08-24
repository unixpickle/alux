#include "executable.hpp"
#include <anarch/assert>

namespace Alux {

namespace x64 {

Executable::Executable(PhysAddr m, PhysSize l) : memory(m), length(l) {
  assert(l % 0x200000 == 0);
  assert(m % 0x200000 == 0);
}

ExecutableMap & Executable::GenerateMap(anarch::UserMap & m) {
  return ExecutableMap::New(*this, m);
}

}

}
