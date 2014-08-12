#ifndef __ALUX_ROUNDROBIN_THREAD_INFO_HPP__
#define __ALUX_ROUNDROBIN_THREAD_INFO_HPP__

#include <anarch/stdint>
#include <ansa/atomic>

namespace OS {

struct ThreadInfo {
  ansa::Atomic<uint64_t> nextTick = 0;
  bool running = false;
};

}

#endif