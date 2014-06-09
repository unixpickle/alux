#ifndef __X64_INIT_HPP__
#define __X64_INIT_HPP__

namespace OS {

namespace x64 {

void InitializeMemory(void * mbootPtr);
void InitializeInterrupts();
void InitializeACPI();
void InitializeAPIC();
void InitializeTime();
void InitializeSMP();
void InitializeTimers();
void InitializeScheduler();

}

}

#endif
