#include <arch/general/console.hpp>
#include <iostream>
#include <new>

namespace OS {

static OutStreamModule gModule;
OutStreamErr cerr;
OutStreamOut cout;
const char * endl = "\n";

void OutStreamModule::InitGlobal() {
  new(&gModule) OutStreamModule();
}

OutStreamModule & OutStreamModule::GetGlobal() {
  return gModule;
}

void OutStreamModule::Initialize() {
  new(&cerr) OutStreamErr;
  new(&cout) OutStreamOut;
}

DepList OutStreamModule::GetDependencies() {
  return DepList(&Console::GetGlobal());
}

OutStream & OutStream::operator<<(const char * ch) {
  return cerr << "OutStream::operator<<("
    << (unsigned long long)ch
    << ") invalid!"
    << endl;
}

OutStream & OutStream::operator<<(int number) {
  return *this << (long long)number;
}

OutStream & OutStream::operator<<(long number) {
  return *this << (long long)number;
}

OutStream & OutStream::operator<<(long long number) {
  if (number < 0) {
    (*this) << "-";
    return (*this) << (unsigned long long)-number;
  } else {
    return (*this) << (unsigned long long)number;
  }
}

OutStream & OutStream::operator<<(unsigned int number) {
  return *this << (unsigned long long)number;
}

OutStream & OutStream::operator<<(unsigned long number) {
  return *this << (unsigned long long)number;
}

OutStream & OutStream::operator<<(unsigned long long number) {
  const char * chars = "0123456789abcdef";
  unsigned char _buf[32] = {0};
  _buf[0] = '0';
  _buf[1] = 'x';
  unsigned char * buf = _buf + 2;
  unsigned char len = 2, i;
  do {
    unsigned char nextDig = (unsigned char)(number & 0xf);
    buf[len++] = chars[nextDig];
    number >>= 4;
  } while (number > 0);
  for (i = 0; i < len / 2; i++) {
    unsigned char a = buf[len - i - 1];
    buf[len - i - 1] = buf[i];
    buf[i] = a;
  }
  (*this) << (const char *)_buf;
  
  return *this;
}

OutStream & OutStreamErr::operator<<(const char * str) {
  Console & console = Console::GetGlobal();
  console.SetColor(Console::ColorRed, true);
  console.PrintString(str);
  return *this;
}

OutStream & OutStreamOut::operator<<(const char * str) {
  Console & console = Console::GetGlobal();
  console.SetColor(Console::ColorGreen, true);
  console.PrintString(str);
  return *this;
}

}
