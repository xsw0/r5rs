#ifndef _R5RS_GETREF_H_
#define _R5RS_GETREF_H_

#include <vector>

#include "Type.h"
#include "GC.h"

namespace r5rs
{
  class GetRef final
  {
  public:
    std::vector<Reference *> operator()(std::monostate value);
    std::vector<Reference *> operator()(nullptr_t value);
    std::vector<Reference *> operator()(bool value);
    std::vector<Reference *> operator()(char value);
    std::vector<Reference *> operator()(int64_t value);
    std::vector<Reference *> operator()(double value);
    std::vector<Reference *> operator()(const std::string & value);
    std::vector<Reference *> operator()(Pair & value);
    std::vector<Reference *> operator()(std::vector<Reference> & value);
  };
}

#endif
