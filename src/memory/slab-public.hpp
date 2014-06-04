extern "C" {
#include <anlock.h>
}

namespace OS {

template <class T, size_t Count>
template <typename... Args>
T * Slab<T, Count>::New(Args... args) {
  anlock_lock(&lock);
  T * res = super::New(args...);
  anlock_unlock(&lock);
  return res;
}

template <class T, size_t Count>
void Slab<T, Count>::Delete(T * x) {
  anlock_lock(&lock);
  super::Delete(x);
  anlock_unlock(&lock);
}

}