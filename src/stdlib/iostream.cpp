#include <arch/general/printing.hpp>
#include <iostream>
#include <new>

namespace OS {

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
  SetColor(PrintColorRed | PrintColorBrightMask);
  PrintString(str);
  return *this;
}

OutStream & OutStreamOut::operator<<(const char * str) {
  SetColor(PrintColorGreen | PrintColorBrightMask);
  PrintString(str);
  return *this;
}

OutStreamErr cerr;
OutStreamOut cout;
const char * endl = "\n";
static bool hasInited = false;

bool HasInitializedOutStream() {
  return hasInited;
}

void InitializeOutStream() {
  new(&cerr) OutStreamErr;
  new(&cout) OutStreamOut;
  hasInited = true;
}

}