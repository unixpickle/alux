#include <anarch/stddef>

void * operator new(size_t s);
void * operator new[](size_t s);
void operator delete(void * p);
void operator delete[](void * p);
