#ifndef _R5RS_TYPE_H_
#define _R5RS_TYPE_H_

#include <cstddef>
#include <cstdint>
#include <utility>
#include <variant>
#include <string>
#include <vector>

namespace r5rs
{
  class Reference;

  using Pair = std::pair<Reference, Reference>;

  using Value = std::variant<
    std::monostate,
    nullptr_t,
    bool,
    char,
    int64_t,
    double,
    std::string,
    Pair,
    std::vector<Reference>
  >;

}

#endif
