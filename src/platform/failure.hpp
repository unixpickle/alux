#ifndef __PLATFORM_FAILURE_H__
#define __PLATFORM_FAILURE_H__

namespace OS {

void Panic(const char * message);
void ForceHalt();

}

#endif
