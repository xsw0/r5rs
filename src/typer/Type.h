#ifndef _R5RS_TYPE_H_
#define _R5RS_TYPE_H_

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <list>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace r5rs
{
  // template<typename T, typename Y>
  // concept Like =
  //   std::invocable< decltype([](Y const &) {}), T > ||
  //   std::invocable< decltype([](Y &&) {}), T > ;

  template<typename T, typename Y>
  concept just = std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<Y>>;

  template<class T>
  concept object = std::is_object_v<T>;

  template <class T>
  concept Value = !std::is_reference_v<T>;
  template <class T>
  concept Reference = std::is_reference_v<T>;
  template <class T>
  concept LvalueReference = std::is_lvalue_reference_v<T>;
  template <class T>
  concept RvalueReference = std::is_rvalue_reference_v<T>;

  template <class T, template <class...> class TC>
  struct is_a: std::false_type {};
  template <class T, template <class...> class TC>
  struct is_a<TC<T>, TC>: std::true_type {};
  template <class T, template <class...> class TC>
  inline constexpr bool is_a_v = is_a<T, TC>::value;
  template <class T, template <class...> class TC>
  concept Is = is_a_v<T, TC>;

  template <class T, template <class...> class TC>
  concept IsNot = !is_a_v<T, TC>;

  template <class R, class F, class... Args>
  concept InvocableR = std::is_invocable_r_v<R, F, Args...>;
  template <class R, class F, class... Args>
  concept RegularInvocableR = InvocableR<R, F, Args...>;

  template <template <class> class RT, class F, class... Args>
  struct is_invocable_r_t
    : std::bool_constant<std::is_invocable_v<F, Args...> &&
    RT<std::invoke_result_t<F, Args...>>::value>
  {
  };
  template <template <class> class RT, class F, class... Args>
  inline constexpr bool is_invocable_r_t_v =
    is_invocable_r_t<RT, F, Args...>::value;

  template <template <class> class RT, class F, class... Args>
  concept InvocableRT = is_invocable_r_t_v<RT, F, Args...>;
  template <template <class> class RT, class F, class... Args>
  concept RegularInvocableRT = InvocableRT<RT, F, Args...>;

  template <template <class> class RF, class F, class... Args>
  struct is_Invocable_r_f
    : std::bool_constant<std::is_invocable_v<F, Args...> &&
    !RF<std::invoke_result_t<F, Args...>>::value>
  {
  };
  template <template <class> class RF, class F, class... Args>
  inline constexpr bool is_Invocable_r_f_v =
    is_Invocable_r_f<RF, F, Args...>::value;

  template <template <class> class RF, class F, class... Args>
  concept InvocableRF = is_Invocable_r_f_v<RF, F, Args...>;
  template <template <class> class RF, class F, class... Args>
  concept RegularInvocableRF = InvocableRF<RF, F, Args...>;

  template <template <class...> class RC, class F, class... Args>
  struct is_invocable_r_c
    : std::bool_constant<std::is_invocable_v<F, Args...> &&
    is_a_v<std::invoke_result_t<F, Args...>, RC>> {};
  template <template <class...> class RC, class F, class... Args>
  inline constexpr bool is_invocable_r_c_v =
    is_invocable_r_c<RC, F, Args...>::value;

  template <template <class...> class RC, class F, class... Args>
  concept InvocableRC = is_invocable_r_c_v<RC, F, Args...>;
  template <template <class...> class RC, class F, class... Args>
  concept RegularInvocableRC = InvocableRC<RC, F, Args...>;

  template <size_t index> auto pack_get(auto && arg, auto &&...args)
  {
    if constexpr (index == 0)
    {
      return std::forward<decltype(arg)>(arg);
    }
    else
    {
      return pack_get<index - 1>(args...);
    }
  }

  template <class... Ts> struct overloaded: Ts... {
    using Ts::operator()...;
  };
  template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

  class InternalGCRef;
  class GCRef;

  using Pair = std::pair<InternalGCRef, InternalGCRef>;

  struct Symbol
  {
    std::string name;
  };

  namespace expression
  {
    class Lambda;
  }
  class Env;

  class ClosureLambda
  {
  public:
    expression::Lambda * lambda;
    std::shared_ptr<Env> env;
  };

  using Primitive = GCRef(*)(std::list<GCRef> &);

  using Vector = std::vector<InternalGCRef>;

  using GCValue = std::variant<
    // std::monostate,
    nullptr_t, bool, char, int64_t, double, std::string, Symbol, Pair, Vector,
    ClosureLambda, Primitive>;

  template <typename Ret, typename... Args>
  using function_ptr = std::shared_ptr<std::function<Ret(Args...)>>;

  auto make_function(auto && func)
    -> std::shared_ptr<decltype(std::function(func))>
  {
    // static std::list<std::shared_ptr<decltype(std::function(func))>> list;
    auto shared =
      std::make_shared<decltype(std::function(func))>(std::function(func));
    // list.push_back(shared);
    assert(shared);
    assert(*shared);
    return shared;
  }
} // namespace r5rs

#endif
