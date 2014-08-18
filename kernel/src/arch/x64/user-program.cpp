#include "user-program.hpp"

namespace OS {

namespace x64 {

UserProgram::UserProgram(PhysAddr _memory, PhysSize _length)
  : memory(_memory), length(_length) {
}

UserProgramMap & UserProgram::CreateMap(anarch::UserMap & map) {
  return UserProgramMap::New(map, *this);
}

PhysAddr UserProgram::GetMemory() const {
  return memory;
}

PhysSize UserProgram::GetLength() const {
  return length;
}

}

}
