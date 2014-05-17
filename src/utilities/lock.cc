#include "lock.hpp"
#include <scheduler/scheduler-init.hpp>
#include <platform/interrupts.hpp>

namespace OS {

ScopeLock::ScopeLock(anlock_t lock) {
  theLock = lock;
  assert(!((uint64_t)lock & 7));
  assert(!HasInitializedScheduler() || GetInterruptsEnabled());
  anlock_lock(theLock);
}

ScopeLock::~ScopeLock() {
  anlock_unlock(theLock);
}

}
