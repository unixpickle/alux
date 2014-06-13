#include <critical>

namespace OS {

ScopeCritical::ScopeCritical() : wasCritical(GetCritical()) {
  if (!wasCritical) SetCritical(true);
}

ScopeCritical::~ScopeCritical() {
  if (!wasCritical) SetCritical(false);
}

}
