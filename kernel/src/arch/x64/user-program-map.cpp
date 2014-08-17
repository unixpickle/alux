#include "user-program-map.hpp"
#include "user-program.hpp"

namespace OS {

namespace x64 {

UserProgramMap & UserProgramMap::New(UserProgram & prog) {
  return *(new UserProgramMap(prog));
}

void * UserProgramMap::GetEntryPoint() {
  return (void *)0x8000000000UL;
}

OS::UserProgram & UserProgramMap::GetUserProgram() {
  return program;
}

bool UserProgramMap::HandlePageFault(VirtAddr, bool) {
  // TODO: this
  return false;
}

void UserProgramMap::Delete() {
  delete this;
}

UserProgramMap::UserProgramMap(UserProgram & prog) : program(prog) { 
}

}

}