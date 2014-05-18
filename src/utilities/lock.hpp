#ifndef __UTILITIES_LOCK_H__
#define __UTILITIES_LOCK_H__

extern "C" {

#include <anlock.h>

}

#include <cassert>

namespace OS {

class ScopeLock {
private:
  anlock_t theLock;
public:
  /**
   * @noncritical
   */
  ScopeLock(anlock_t lock);
  
  /**
   * @ambicritical
   */
  ~ScopeLock();
};

class ScopeCriticalLock {
private:
  anlock_t theLock;

public:
  /**
   * @critical
   */
  ScopeCriticalLock(anlock_t lock);
  
  /**
   * @ambicritical
   */
  ~ScopeCriticalLock();
};

}

#endif
