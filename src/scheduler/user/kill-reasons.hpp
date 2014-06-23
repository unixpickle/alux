#ifndef __SCHEDULER_KILL_REASONS_HPP__
#define __SCHEDULER_KILL_REASONS_HPP__

namespace OS {

namespace KillReasons {

enum Reasons {
  Natural,
  UserError,
  UnownedMemory,
  InvalidSyscall,
  MemoryFault
};

}

}

#endif
