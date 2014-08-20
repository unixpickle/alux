#ifndef __ALUX_PAGE_FAULT_HPP__
#define __ALUX_PAGE_FAULT_HPP__

#include <anarch/api/page-delegate>

namespace Alux {

void HandlePageFault(VirtAddr, bool); // @critical -> @noncritical -> @critical

}

#endif
