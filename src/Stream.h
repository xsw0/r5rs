#ifndef _R5RS_STREAM_H_
#define _R5RS_STREAM_H_

#include "Type.h"

namespace r5rs {
template <typename T> class IStream final {
public:
  IStream(function_ptr<Try<T>> fun)
      : func(fun), buffer(std::make_shared<std::vector<T>>()) {
    assert(func);
    assert(*func);
  }

  IStream(std::vector<T> init)
      : buffer(std::make_shared<std::vector<T>>(std::move(init))) {}

  std::ptrdiff_t current() const { return cur; }

  Try<T> operator[](std::ptrdiff_t index) {
    if (cur + index < 0) {
      return Error{"access negative position."};
    }
    while (cur + index >= buffer->size()) {
      Try<T> t;
      if (!func || !(t = std::invoke(*func))) {
        return Error{"Eof"};
      }
      buffer->push_back(*t);
    }
    return (*buffer)[cur + index];
  }

  bool eof() { return !operator[](0); }

  IStream &operator+=(std::ptrdiff_t index) {
    cur += index;
    return *this;
  }
  IStream &operator-=(std::ptrdiff_t index) {
    cur -= index;
    return *this;
  }
  IStream operator+(std::ptrdiff_t index) const {
    return IStream(*this) += index;
  }
  IStream operator-(std::ptrdiff_t index) const {
    return IStream(*this) -= index;
  }

  template <typename R, typename Z>
  auto foldl(function_ptr<R, Z, T> op, Z init) {
    for (size_t i = 0; operator[](i); ++i) {
      init = std::invoke(*op, init, *operator[](i));
    }
    return init;
  }

  operator std::list<T>() {
    std::list<T> res;
    for (size_t i = 0; operator[](i); ++i) {
      res.push_back(*operator[](i));
    }
    return res;
  }

  std::list<T> list() { return operator std::list<T>(); }

private:
  function_ptr<Try<T>> func;
  std::shared_ptr<std::vector<T>> buffer;
  std::ptrdiff_t cur = 0;
};
} // namespace r5rs

#endif
