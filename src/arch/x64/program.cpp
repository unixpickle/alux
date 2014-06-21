#include <cstdint>
#include <cstddef>

#include <iostream> // TODO: delete this

namespace OS {

namespace x64 {

static uint64_t GetBinEnd() {
  return *((uint64_t *)0x100020);
}

void * GetProgramStart() {
  return (void *)(GetBinEnd() + 0x8);
}

size_t GetProgramSize() {
  uint64_t * ptr = (uint64_t *)GetBinEnd();
  return (size_t)*ptr;
}

}

}
