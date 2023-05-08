#ifndef _R5RS_Parse_H_
#define _R5RS_Parse_H_

#include <list>
#include <cstddef>
#include <iostream>
#include <optional>
#include <vector>
#include <functional>
#include <memory>
#include <tuple>

#include "Type.h"
#include "Try.h"
#include "Stream.h"

namespace r5rs
{
  struct Char
  {
    char ch;
    size_t line = 0;
    size_t col = 0;
  };

  template<typename Input, typename Output>
  using ParserResult = Try<std::pair<Output, IStream<Input>>>;

  template<typename Input, typename Output>
  class Parser
  {
  public:
    using input_t = Input;
    using output_t = Output;
    using result_t = ParserResult<input_t, output_t>;
    using function_t = function_ptr<result_t, IStream<input_t>>;

    Parser(function_t func): func(func) { assert(func && *func); };

    operator bool() const { return func && *func; }

    ParserResult<Input, Output> operator()(IStream<Input> input) const
    {
      assert(func && *func);
      return std::invoke(*func, input);
    }

    template<typename R>
    Parser<Input, R> operator>>=(function_ptr<Parser<Input, R>, Output> f);

    template<typename R>
    Parser<Input, R> as(R value);

    template<typename T>
    Parser<Input, T> as()
    {
      return map<T>(
        make_function(
          [](Output val) {
            return T{ val };
          }
        )
      );
    }

    Parser<Input, std::shared_ptr<Output>> shared()
    {
      return map<std::shared_ptr<Output>>(
        make_function(
          [](Output val) {
            return std::make_shared<Output>(val);
          }
        )
      );
    }

    template<typename R>
    Parser<Input, R> map(function_ptr<R, Output> f);

    Parser<Input, Output> filter(function_ptr<bool, Output> f);

    template<typename R>
    Parser<Input, Output> peek(Parser<Input, R> parser);

    Parser<Input, std::list<Output>> many();
    Parser<Input, std::list<Output>> some();
    Parser<Input, std::optional<Output>> maybe();

    function_t func;
  };

  template<typename Input, typename Output>
  Parser(function_ptr<ParserResult<Input, Output>, IStream<Input>>) -> Parser<Input, Output>;

  template<typename Input, typename Output>
  template<typename R>
  inline auto Parser<Input, Output>::operator>>=(function_ptr<Parser<Input, R>, Output> f)
    -> Parser<Input, R>
  {
    assert(*this);
    assert(f && *f);
    return make_function(
      [=, *this](IStream<Input> input) {
        assert(*this);
        assert(f && *f);
        return std::invoke(*func, input)
          >>= [=, *this](auto && pair) {
          assert(*this);
          assert(f && *f);
          return std::invoke(std::invoke(*f, pair.first), pair.second);
        };
      }
    );
  }

  template<typename R, typename Input, typename ... Outputs>
  inline Parser<Input, R> combine(function_ptr<R, Outputs ...> f, Parser<Input, Outputs> ... args)
  {
    (..., assert(args));
    return make_function(
      [=](IStream<Input> input) {
        (..., assert(args));
        return (
          Try(std::make_pair(std::tuple<>(), input)) >>= ... >>=
          [=](auto && pair) {
            (..., assert(args));
            return std::invoke(args, pair.second) >>= [=](auto && p) {
              return Try(
                std::make_pair(
                  std::tuple_cat(pair.first, std::tuple(p.first)),
                  p.second
                )
              );
            };
          }
        ) >>=
          [=](auto && pair) {
            (..., assert(args));
            return Try(
              std::make_pair(std::apply(*f, pair.first), pair.second)
            );
          };
      }
    );
  }

  template<size_t index, typename Input, typename ... Args>
  inline auto select(Parser<Input, Args> ... parser)
    -> Parser<Input, typename std::tuple_element<index, std::tuple<Args...>>::type>
  {
    (..., assert(parser));
    return combine(
      make_function(
        [=](Args ... args) {
          (..., assert(parser));
          return pack_get<index>(args...);
        }
      ),
      parser ...
    );
  }

  template<typename Input, typename Output>
  template<typename R>
  inline Parser<Input, R> Parser<Input, Output>::as(R value)
  {
    assert(*this);
    return combine(make_function([=, *this](Output) { assert(*this); return value; }), *this);
  }

  template<typename Input, typename Output>
  template<typename R>
  inline Parser<Input, R> Parser<Input, Output>::map(function_ptr<R, Output> f)
  {
    assert(*this);
    assert(f && *f);
    return combine(f, *this);
  }

  template<typename Input, typename Output>
  inline Parser<Input, Output> Parser<Input, Output>::filter(function_ptr<bool, Output> f)
  {
    assert(*this);
    assert(f && *f);
    return make_function(
      [=, *this](IStream<Input> input) -> ParserResult<Input, Output> {
        assert(*this);
        assert(f && *f);
        auto res = std::invoke(*func, input);
        if (!res) { return res; }
        if (!std::invoke(*f, res->first)) { return Error{ "filter fail." }; }
        return res;
      }
    );
  }

  template<typename Input, typename Output>
  template<typename R>
  inline Parser<Input, Output> Parser<Input, Output>::peek(Parser<Input, R> parser)
  {
    assert(*this);
    assert(parser);
    return make_function(
      [=, *this](IStream<Input> input) -> ParserResult<Input, Output> {
        assert(*this);
        assert(parser);
        return std::invoke(*func, input) >>=
          [=, *this](auto && pair) -> ParserResult<Input, Output> {
          assert(*this);
          assert(parser);
          if (parser(pair.second))
          {
            return pair;
          }
          else
          {
            return Error{ "peek fail." };
          }
        };
      }
    );
  }

  template<typename Input, typename Output>
  inline Parser<Input, std::list<Output>> Parser<Input, Output>::many()
  {
    assert(*this);
    return make_function(
      [=, *this](IStream<Input> input) -> ParserResult<Input, std::list<Output>> {
        assert(*this);
        std::list<Output> output;
        result_t pair;
        while ((pair = std::invoke(*func, input)))
        {
          input = pair->second;
          output.push_back(pair->first);
        }
        return std::make_pair(output, input);
      }
    );
  }

  template<typename Input, typename Output>
  inline Parser<Input, std::list<Output>> Parser<Input, Output>::some()
  {
    assert(*this);
    return make_function(
      [=, *this](IStream<Input> input) -> ParserResult<Input, std::list<Output>> {
        assert(*this);
        std::list<Output> output;
        result_t pair;
        while ((pair = std::invoke(*func, input)))
        {
          input = pair->second;
          output.push_back(pair->first);
        }
        if (output.empty()) { return Error{ "not find any match." }; }
        return std::make_pair(output, input);
      }
    );
  }

  template<typename Input, typename Output>
  inline Parser<Input, std::optional<Output>> Parser<Input, Output>::maybe()
  {
    assert(*this);
    return make_function(
      [=, *this](IStream<Input> input) -> ParserResult<Input, std::optional<Output>> {
        assert(*this);
        std::optional<Output> output;
        result_t pair = std::invoke(*func, input);
        if (pair)
        {
          input = pair->second;
          output = pair->first;
        }
        return std::make_pair(output, input);
      }
    );
  }

  template<typename Input, typename Output>
  inline auto operator||(Parser<Input, Output> lhs, Parser<Input, Output> rhs)
    -> Parser<Input, Output>
  {
    assert(lhs);
    assert(rhs);
    return make_function(
      [=](IStream<Input> input) -> ParserResult<Input, Output> {
        assert(lhs);
        assert(rhs);
        return std::invoke(lhs, input) || std::invoke(rhs, input);
      }
    );
  }

  IStream<Char> & cinIStream();
} // namespace r5rs

#endif
