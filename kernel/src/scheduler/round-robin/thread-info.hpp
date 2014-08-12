#ifndef __ALUX_ROUNDROBIN_THREAD_INFO_HPP__
#define __ALUX_ROUNDROBIN_THREAD_INFO_HPP__

#include <anarch/stdint>

namespace OS {

struct ThreadInfo {
  uint64_t nextTick = 0;
};

}

#endif