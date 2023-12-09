#ifndef R5RS_VALUE_REF_H
#define R5RS_VALUE_REF_H

#include <type_traits>

namespace r5rs
{
  template<typename T>
  concept object = std::is_object_v<T>;

  template<object T>
  class value_ref final
  {
  public:
    ~value_ref() { delete data; }
  private:
    constexpr explicit value_ref(T * p) noexcept: data{ p } { assert(p); }
  public:
    template<typename U, typename ... Args>
    friend value_ref<U> make_value(Args && ... args)
      noexcept (std::is_nothrow_constructible_v<U, Args && ...>);

    value_ref(value_ref const & ref)
      noexcept(std::is_nothrow_copy_constructible_v<T>)
      : data{ new T{ *ref } }
    {
    }

    value_ref(value_ref && ref)
      noexcept(std::is_nothrow_move_constructible_v<T>)
      : data{ new T{ std::move(*ref) } }
    {
    }

    value_ref & operator=(value_ref const & ref)
      noexcept(std::is_nothrow_copy_assignable_v<T>)
    {
      *data = *ref;
      return *this;
    }

    value_ref & operator=(value_ref && ref)
      noexcept(std::is_nothrow_move_assignable_v<T>)
    {
      *data = std::move(*ref);
      return *this;
    }

    T & operator*() const noexcept { return *data; }
    T * operator->() const noexcept { return data; }
  private:
    T * const data;
  };

  template<typename T, typename ... Args>
  value_ref<T> make_value(Args && ... args)
    noexcept (std::is_nothrow_constructible_v<T, Args && ...>)
  {
    return value_ref<T>{new T{ std::forward<Args>(args)... }};
  }
}

#endif
