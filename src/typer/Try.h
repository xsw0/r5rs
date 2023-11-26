#ifndef _R5RS_TRY_H_
#define _R5RS_TRY_H_

#include <numeric>

#include "Type.h"

namespace r5rs {
  struct Error {
    std::string message;
    std::ptrdiff_t level = std::numeric_limits<std::ptrdiff_t>::max();
  };

  template <object T> class Try {
  public:
    static_assert(!std::is_same_v<Error, std::remove_cvref_t<T>>);
    using type = T;

    Try() : value(Error{ "" }) {};

    Try(Try&&) noexcept = default;
    Try(const Try&) = default;
    Try& operator=(const Try& t) = default;
    Try& operator=(Try&& t) = default;

    Try(Error err) : value(std::move(err)) {};
    Try(T value) : value(std::move(value)) {}

    std::string error() const { return std::get<Error>(value).message; }

    std::variant<T, Error> value;

    operator bool() { return std::holds_alternative<T>(value); }

    T operator*() {
      assert(*this);
      return std::get<T>(value);
    }

    T* operator->() {
      assert(*this);
      return &std::get<T>(value);
    }
  };

  template <typename T> Try(Try<T>&&) -> Try<T>;
  template <typename T> Try(T&&) -> Try<T>;

  template <object T>
  auto operator>>=(Try<T> t, auto&& f) -> std::invoke_result_t<decltype(f), T>
    requires RegularInvocableRC<Try, decltype(f), T>
  {
    using result_t = std::invoke_result_t<decltype(f), T>;
    if (std::holds_alternative<Error>(t.value)) {
      return result_t(std::move(std::get<Error>(t.value)));
    }
    return std::invoke(std::forward<decltype(f)>(f), std::get<T>(t.value));
  }

  template <object T> inline Try<T> operator||(Try<T> lhs, Try<T> rhs) {
    static_assert(!std::is_reference_v<T>);
    return lhs ? lhs : rhs;
  };
} // namespace r5rs

#endif
