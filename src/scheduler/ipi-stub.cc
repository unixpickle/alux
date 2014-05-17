#include "ipi-stub.hpp"

namespace OS {

void * HandleIPI(void * object) {
  cout << "got IPI " << (uintptr_t)object << endl;
  return NULL;
}

}
