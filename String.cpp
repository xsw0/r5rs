#include "String.h"

#include <variant>
#include <sstream>

#include "GC.h"

using namespace r5rs;

std::string r5rs::String::operator()(std::monostate value)
{
  return "monostate";
}

std::string r5rs::String::operator()(nullptr_t value)
{
  return "nullptr";
}

std::string r5rs::String::operator()(bool value)
{
  return value ? "true" : "false";
}

std::string r5rs::String::operator()(char value)
{
  return "'" + std::to_string(value) + "'";
}

std::string r5rs::String::operator()(int64_t value)
{
  return std::to_string(value);
}

std::string r5rs::String::operator()(double value)
{
  return std::to_string(value);
}

std::string r5rs::String::operator()(const std::string & value)
{
  return '"' + value + '"';
}

template< typename T >
std::string int_to_hex(T i)
{
  std::stringstream stream;
  stream << "0x"
    << std::setfill('0') << std::setw(sizeof(T) * 2)
    << std::hex << i;
  return stream.str();
}

std::string r5rs::String::operator()(const Pair & value)
{
  return "(" + int_to_hex((intptr_t) & *value.first) + " . " + int_to_hex((intptr_t) & *value.second) + ")";
}

std::string r5rs::String::operator()(const std::vector<Reference> & value)
{
  // std::string result("#(");
  // for (auto && v : value)
  // {
  //   result += std::visit(*this, v.value) + " ";
  // }
  // result.back() = ')';
  // return result;;
  return "#";
}
