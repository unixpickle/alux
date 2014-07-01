#include <lock>
#include <critical>

extern "C" {

#include <anlock.h>

}

namespace OS {

void LockHold(uint64_t * lock) {
  anlock_lock(lock);
}

void LockRelease(uint64_t * lock) {
  anlock_unlock(lock);
}

ScopeLock::ScopeLock(uint64_t * _lock) : lock(_lock) {
  AssertNoncritical();
  anlock_lock(lock);
}

ScopeLock::~ScopeLock() {
  anlock_unlock(lock);
}

ScopeCriticalLock::ScopeCriticalLock(uint64_t * _lock) : lock(_lock) {
  AssertCritical();
  anlock_lock(lock);
}

ScopeCriticalLock::~ScopeCriticalLock() {
  anlock_unlock(lock);
}

}
