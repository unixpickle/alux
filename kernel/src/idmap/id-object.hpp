#ifndef __ALUX_ID_OBJECT_HPP__
#define __ALUX_ID_OBJECT_HPP__

#include "identifier.hpp"
#include "id-map.hpp"

namespace Alux {

/**
 * Generally, you may wish to subclass [IdObject] if you are storing an object
 * in an [IdMap].
 */
class IdObject {
public:  
  inline Identifier GetIdentifier() const {
    return identifier;
  }
  
protected:
  template <class T>
  friend class IdMap;
  
  inline void SetIdentifier(Identifier anId) {
    identifier = anId;
  }
  
private:
  Identifier identifier;
};

}

#endif
