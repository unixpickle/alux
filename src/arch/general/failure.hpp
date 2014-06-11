#ifndef __GENERAL_FAILURE_HPP__
#define __GENERAL_FAILURE_HPP__

#include <common>

namespace OS {

void Panic(const char * msg) OS_NORETURN;

}

#endif
