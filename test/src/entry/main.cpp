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
  std::puts("hello from user-space!");
}
