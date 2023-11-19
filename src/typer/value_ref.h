#ifndef _R5RS_VAL_H_
#define _R5RS_VAL_H_

namespace r5rs
{
  template<typename T>
  class value_ref
  {
  public:
    ~value_ref() { delete data; }

    value_ref(T val): data{ new T{ std::move(val) } } {}

    template<typename ... Args>
    value_ref(Args ... args) : value_ref(T{ std::forward<Args>(args)... }) {}

    value_ref(value_ref const & val): value_ref{ *val.data } {};
    value_ref(value_ref && val): value_ref{ std::move(*val.data) } {}

    value_ref & operator=(value_ref const & val)
    {
      *data = *val.data; return *this;
    }

    value_ref & operator=(value_ref && val)
    {
      *data = std::move(*val.data); return *this;
    }

    template<typename Y>
    value_ref & operator=(Y && val)
    {
      *data = std::forward<Y>(val); return *this;
    }

    operator T() const { return *data; }
  private:
    T * const data;
  };
}

#endif
