#include <iostream>

void Main();

extern "C" {

void main() {
  Main();
}

void _main() {
  Main();
}

}

void Main() {
  std::puts("hello from user-space!\n");
  std::puts("screw the man!");
  std::puts((const char *)0x1337);
  std::puts("I stuck it to the man!");
}
