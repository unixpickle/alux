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
  ScopeLock(anlock_t lock);
  ~ScopeLock();
};

}

#endif