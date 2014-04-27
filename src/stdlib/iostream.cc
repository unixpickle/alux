#include <iostream>

namespace OS {

OutStream & OutStream::operator<<(const char * ch) {
  return cerr << "OutStream::operator<<("
    << (unsigned long long)ch
    << ") invalid!"
    << endl;
}


OutStream & OutStream::operator<<(long long number) {
  if (number < 0) {
    (*this) << "-";
    return (*this) << (unsigned long long)-number;
  } else {
    return (*this) << (unsigned long long)number;
  }
}

OutStream & OutStream::operator<<(unsigned long long number) {
  const char * chars = "0123456789abcdef";
  unsigned char buf[32] = {0};
  buf[0] = '0';
  buf[1] = 'x';
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
  (*this) << (const char *)buf;
  
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

void InitializeStreams() {
  new (&cerr) OutStreamErr();
  new (&cerr) OutStreamOut();
}

}