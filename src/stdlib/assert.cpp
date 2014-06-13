#include <panic>

extern "C" {

int __assert(const char * msg) {
  OS::Panic(msg);
  return 0;
}

}
