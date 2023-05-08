#include "GetRef.h"

using namespace r5rs;

std::vector<Reference *> r5rs::GetRef::operator()(std::monostate value)
{
  return std::vector<Reference *>();
}

std::vector<Reference *> r5rs::GetRef::operator()(nullptr_t value)
{
  return std::vector<Reference *>();
}

std::vector<Reference *> r5rs::GetRef::operator()(bool value)
{
  return std::vector<Reference *>();
}

std::vector<Reference *> r5rs::GetRef::operator()(char value)
{
  return std::vector<Reference *>();
}

std::vector<Reference *> r5rs::GetRef::operator()(int64_t value)
{
  return std::vector<Reference *>();
}

std::vector<Reference *> r5rs::GetRef::operator()(double value)
{
  return std::vector<Reference *>();
}

std::vector<Reference *> r5rs::GetRef::operator()(const std::string & value)
{
  return std::vector<Reference *>();
}

std::vector<Reference *> r5rs::GetRef::operator()(std::pair<Reference, Reference> & value)
{
  return std::vector<Reference *>{ &value.first, & value.second };
}

std::vector<Reference *> r5rs::GetRef::operator()(std::vector<Reference> & value)
{
  std::vector<Reference *> children;
  children.reserve(value.size());
  for (auto && ref : value)
  {
    children.push_back(&ref);
  }
  return children;
}
