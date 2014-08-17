#include "user-program.hpp"

namespace OS {

namespace x64 {

UserProgramMap & UserProgram::CreateMap() {
  return UserProgramMap::New(*this);
}

}

}
