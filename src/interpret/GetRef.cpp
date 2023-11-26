#include "GetRef.h"

using namespace r5rs;

std::vector<InternalGCRef*>
r5rs::GetRef::operator()(std::monostate value) {
  return std::vector<InternalGCRef*>();
}

std::vector<InternalGCRef*> r5rs::GetRef::operator()(nullptr_t value) {
  return std::vector<InternalGCRef*>();
}

std::vector<InternalGCRef*> r5rs::GetRef::operator()(bool value) {
  return std::vector<InternalGCRef*>();
}

std::vector<InternalGCRef*> r5rs::GetRef::operator()(char value) {
  return std::vector<InternalGCRef*>();
}

std::vector<InternalGCRef*> r5rs::GetRef::operator()(int64_t value) {
  return std::vector<InternalGCRef*>();
}

std::vector<InternalGCRef*> r5rs::GetRef::operator()(double value) {
  return std::vector<InternalGCRef*>();
}

std::vector<InternalGCRef*>
r5rs::GetRef::operator()(const std::string& value) {
  return std::vector<InternalGCRef*>();
}

std::vector<InternalGCRef*> r5rs::GetRef::operator()(const Symbol& value) {
  return std::vector<InternalGCRef*>();
}

std::vector<InternalGCRef*> r5rs::GetRef::operator()(Pair& value) {
  return std::vector<InternalGCRef*>{&value.first, & value.second};
}

std::vector<InternalGCRef*>
r5rs::GetRef::operator()(std::vector<InternalGCRef>& value) {
  std::vector<InternalGCRef*> children;
  children.reserve(value.size());
  for (auto&& ref : value) {
    children.push_back(&ref);
  }
  return children;
}

std::vector<InternalGCRef*>
r5rs::GetRef::operator()(ClosureLambda lambda) {
  return std::vector<InternalGCRef*>();
}

std::vector<InternalGCRef*> r5rs::GetRef::operator()(Primitive primitive) {
  return std::vector<InternalGCRef*>();
}
