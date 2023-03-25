#ifndef _R5RS_STRING_H_
#define _R5RS_STRING_H_

#include <string>
#include <variant>
#include <iostream>
#include <iomanip>

#include "Type.h"

namespace r5rs
{
  class String
  {
  public:
    std::string operator()(std::monostate value);
    std::string operator()(nullptr_t value);
    std::string operator()(bool value);
    std::string operator()(char value);
    std::string operator()(int64_t value);
    std::string operator()(double value);
    std::string operator()(const std::string & value);
    std::string operator()(const Pair & value);
    std::string operator()(const std::vector<Reference> & value);
  };
}

#endif
