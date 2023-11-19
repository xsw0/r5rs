#ifndef _R5RS_TYPE_H_
#define _R5RS_TYPE_H_

#include <cstddef>
#include <cstdint>
#include <utility>
#include <variant>
#include <string>
#include <vector>
#include <type_traits>
#include <list>

namespace r5rs
{
  template<class T>
  concept value_concept = !std::is_reference_v<T>;
  template<class T>
  concept reference_concept = std::is_reference_v<T>;
  template<class T>
  concept lvalue_reference_concept = std::is_lvalue_reference_v<T>;
  template<class T>
  concept rvalue_reference_concept = std::is_rvalue_reference_v<T>;

  template<class T, template<class ...> class C>
  struct is_a: std::false_type {};
  template<class T, template<class ...> class C>
  struct is_a<C<T>, C>: std::true_type {};
  template<class T, template<class ...> class C>
  inline constexpr bool is_a_v = is_a<T, C>::value;

  template<class R, class F, class... Args>
  concept invocable_r = std::is_invocable_r_v<R, F, Args ...>;
  template<class R, class F, class... Args>
  concept regular_invocable_r = invocable_r<R, F, Args ...>;

  template<template<class> class RT, class F, class... Args>
  struct is_invocable_rt: std::bool_constant<
    std::is_invocable_v<F, Args ...> &&
    RT<std::invoke_result_t<F, Args ...>>::value
  >
  {
  };
  template<template<class> class RT, class F, class... Args>
  inline constexpr bool is_invocable_rt_v = is_invocable_rt<RT, F, Args ...>::value;

  template<template<class> class RT, class F, class... Args>
  concept invocable_rt = is_invocable_rt_v<RT, F, Args ...>;
  template<template<class> class RT, class F, class... Args>
  concept regular_invocable_rt = invocable_rt<RT, F, Args ...>;

  template<template<class> class RF, class F, class... Args>
  struct is_invocable_rf: std::bool_constant<
    std::is_invocable_v<F, Args ...> &&
    !RF<std::invoke_result_t<F, Args ...>>::value
  >
  {
  };
  template<template<class> class RF, class F, class... Args>
  inline constexpr bool is_invocable_rf_v = is_invocable_rf<RF, F, Args ...>::value;

  template<template<class> class RF, class F, class... Args>
  concept invocable_rf = is_invocable_rf_v<RF, F, Args ...>;
  template<template<class> class RF, class F, class... Args>
  concept regular_invocable_rf = invocable_rf<RF, F, Args ...>;

  template<template<class ...> class RC, class F, class... Args>
  struct is_invocable_rc: std::bool_constant<
    std::is_invocable_v<F, Args ...> &&
    is_a_v<std::invoke_result_t<F, Args ...>, RC>
  >
  {
  };
  template<template<class ...> class RC, class F, class... Args>
  inline constexpr bool is_invocable_rc_v = is_invocable_rc<RC, F, Args ...>::value;

  template<template<class ...> class RC, class F, class... Args>
  concept invocable_rc = is_invocable_rc_v<RC, F, Args ...>;
  template<template<class ...> class RC, class F, class... Args>
  concept regular_invocable_rc = invocable_rc<RC, F, Args ...>;

  template<size_t index>
  auto pack_get(auto && arg, auto && ... args)
  {
    if constexpr (index == 0)
    {
      return std::forward<decltype(arg)>(arg);
    }
    else
    {
      return pack_get<index - 1>(args ...);
    }
  }

  template<class... Ts>
  struct overloaded: Ts... { using Ts::operator()...; };
  template<class... Ts>
  overloaded(Ts...) -> overloaded<Ts...>;

  class InternalReference;
  class Reference;

  using Pair = std::pair<InternalReference, InternalReference>;

  struct Symbol
  {
    std::string name;
  };

  namespace expression { class Lambda; }
  class Env;

  class ClosureLambda
  {
  public:
    expression::Lambda * lambda;
    std::shared_ptr<Env> env;
  };

  using Primitive = Reference(*)(std::list<Reference> &);

  using Vector = std::vector<InternalReference>;

  using Value = std::variant<
    // std::monostate,
    nullptr_t,
    bool,
    char,
    int64_t,
    double,
    std::string,
    Symbol,
    Pair,
    Vector,
    ClosureLambda,
    Primitive
  >;

  template<typename Ret, typename ... Args>
  using function_ptr = std::shared_ptr<std::function<Ret(Args ...)>>;

  auto make_function(auto && func)
    -> std::shared_ptr<decltype(std::function(func))>
  {
    // static std::list<std::shared_ptr<decltype(std::function(func))>> list;
    auto shared = std::make_shared<decltype(std::function(func))>(std::function(func));
    // list.push_back(shared);
    assert(shared);
    assert(*shared);
    return shared;
  }
} // namespace r5rs

#endif
