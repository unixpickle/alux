#include "app.hpp"

namespace OS {

namespace x64 {

App::App(PhysAddr _memory, PhysSize _length)
  : memory(_memory), length(_length) {
}

AppCodeMap & App::CreateMap(anarch::UserMap & map) {
  return AppCodeMap::New(map, *this);
}

PhysAddr App::GetMemory() const {
  return memory;
}

PhysSize App::GetLength() const {
  return length;
}

}

}
