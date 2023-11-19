#ifndef _R5RS_GET_REF_H_
#define _R5RS_GET_REF_H_

#include <vector>

#include "Type.h"
#include "GC.h"

namespace r5rs
{
  class GetRef final
  {
  public:
    std::vector<InternalReference *> operator()(std::monostate value);
    std::vector<InternalReference *> operator()(nullptr_t value);
    std::vector<InternalReference *> operator()(bool value);
    std::vector<InternalReference *> operator()(char value);
    std::vector<InternalReference *> operator()(int64_t value);
    std::vector<InternalReference *> operator()(double value);
    std::vector<InternalReference *> operator()(const std::string & value);
    std::vector<InternalReference *> operator()(const Symbol & value);
    std::vector<InternalReference *> operator()(Pair & value);
    std::vector<InternalReference *> operator()(std::vector<InternalReference> & value);
    std::vector<InternalReference *> operator()(ClosureLambda lambda);
    std::vector<InternalReference *> operator()(Primitive primitive);
  };
} // namespace r5rs

#endif
