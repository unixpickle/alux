#ifndef __ALUX_UNUSED_ID_FINDER_HPP__
#define __ALUX_UNUSED_ID_FINDER_HPP__

#include "identifier.hpp"

namespace Alux {

/**
 * An abstract list of free identifiers.
 */
class UnusedIdFinder {
public:
  /**
   * Find a range of identifiers lower than [upperBound] that are not set in
   * some sort of map.
   *
   * @noncritical
   */
  virtual bool FindUnusedIds(Identifier upperBound, Identifier & startOut,
                             Identifier & endOut) = 0;
};

}

#endif
