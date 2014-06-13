#include <lock>

namespace OS {

template <class T, size_t Count>
template <typename... Args>
T * Slab<T, Count>::New(Args... args) {
  ScopeLock scope(&lock);
  T * res = super::New(args...);
  return res;
}

template <class T, size_t Count>
void Slab<T, Count>::Delete(T * x) {
  ScopeLock scope(&lock);
  super::Delete(x);
}

}
