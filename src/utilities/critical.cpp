#include <utilities/critical.hpp>

namespace OS {

ScopeCritical::ScopeCritical() : wasCritical(GetCritical()) {
  if (!wasCritical) SetCritical(true);
}

ScopeCritical::~ScopeCritical() {
  if (!wasCritical) SetCritical(false);
}

}
