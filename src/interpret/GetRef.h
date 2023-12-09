#ifndef R5RS_GET_REF_H
#define R5RS_GET_REF_H

#include <vector>

#include "GC.h"
#include "Type.h"

namespace r5rs {
  class GetRef final {
  public:
    std::vector<InternalGCRef*> operator()(std::monostate value);
    std::vector<InternalGCRef*> operator()(nullptr_t value);
    std::vector<InternalGCRef*> operator()(bool value);
    std::vector<InternalGCRef*> operator()(char value);
    std::vector<InternalGCRef*> operator()(int64_t value);
    std::vector<InternalGCRef*> operator()(double value);
    std::vector<InternalGCRef*> operator()(const std::string& value);
    std::vector<InternalGCRef*> operator()(const Symbol& value);
    std::vector<InternalGCRef*> operator()(Pair& value);
    std::vector<InternalGCRef*>
      operator()(std::vector<InternalGCRef>& value);
    std::vector<InternalGCRef*> operator()(ClosureLambda lambda);
    std::vector<InternalGCRef*> operator()(Primitive primitive);
  };
} // namespace r5rs

#endif
