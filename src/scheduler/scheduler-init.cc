#include "scheduler-init.hpp"

namespace OS {

static bool inited = false;

bool HasInitializedScheduler() {
  return inited;
}

void InitializeScheduler() {
  inited = true;
}

}
