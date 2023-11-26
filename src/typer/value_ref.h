#ifndef _R5RS_VAL_H_
#define _R5RS_VAL_H_

#include <Type.h>

namespace r5rs
{
  template<object T>
  class value_ref
  {
  public:
    ~value_ref() { delete data; }

    value_ref(value_ref<T> const & ref)
      noexcept(std::is_nothrow_copy_constructible_v<T>)
      : data{ new T{ *ref.data } }
    {
    }

    value_ref(value_ref<T> && ref)
      noexcept(std::is_nothrow_move_constructible_v<T>)
      : data{ new T{ std::move(*ref.data) } }
    {
    }

    value_ref(T const & val)
      noexcept(std::is_nothrow_copy_constructible_v<T>)
      : data{ new T{ val } }
    {
    }

    value_ref(T && val)
      noexcept(std::is_nothrow_move_constructible_v<T>)
      : data{ new T{ std::move(val) } }
    {
    }

    template<typename ... Args>
    explicit value_ref(Args && ... args)
      noexcept(std::is_nothrow_constructible_v<T, Args...>)
      : data{ new T{ std::forward<Args>(args)... } }
    {
    }

    decltype(auto) operator=(value_ref<T> const & ref) const
      noexcept(std::is_nothrow_copy_assignable_v<T>)
    {
      *data = *ref.data;
      return *this;
    }

    decltype(auto) operator=(value_ref<T> && ref) const
      noexcept(std::is_nothrow_move_assignable_v<T>)
    {
      *data = std::move(*ref.data);
      return *this;
    }

    decltype(auto) operator=(T const & val) const
      noexcept(std::is_nothrow_copy_assignable_v<T>)
    {
      *data = val;
      return *this;
    }

    decltype(auto) operator=(T && val) const
      noexcept(std::is_nothrow_move_assignable_v<T>)
    {
      *data = std::move(val);
      return *this;
    }

    operator T() const
    {
      return *data;
    }
  private:
    T * const data;
  };
}

#endif
