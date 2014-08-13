#include "new.hpp"
#include <anarch/api/domain>
#include <anarch/critical>

void * operator new(size_t s) {
  AssertNoncritical();
  void * result;
  if (!anarch::Domain::GetCurrent().Alloc(result, s)) {
    return NULL;
  }
  return result;
}

void * operator new[](size_t s) {
  AssertNoncritical();
  void * result;
  if (!anarch::Domain::GetCurrent().Alloc(result, s)) {
    return NULL;
  }
  return result;
}

void operator delete(void * p) {
  AssertNoncritical();
  anarch::Domain::GetCurrent().Free(p);
}

void operator delete[](void * p) {
  AssertNoncritical();
  anarch::Domain::GetCurrent().Free(p);
}
