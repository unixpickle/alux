#include "critical.hpp"

namespace OS {

ScopeCritical::ScopeCritical() {
  wereEnabled = GetInterruptsEnabled();
  if (wereEnabled) {
    SetInterruptsEnabled(false);
  }
}

ScopeCritical::~ScopeCritical() {
  SetInterruptsEnabled(wereEnabled);
}

}
