#ifndef _R5RS_Parse_H_
#define _R5RS_Parse_H_

#include <cstddef>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <optional>
#include <tuple>
#include <vector>

#include "Stream.h"
#include "Try.h"
#include "Type.h"

namespace r5rs {
struct Char {
  char ch;
  size_t line = 0;
  size_t col = 0;
};

template <typename Input, typename Output>
using ParserResult = Try<std::pair<Output, IStream<Input>>>;

template <typename Input, typename Output>
class Parser : public std::enable_shared_from_this<Parser<Input, Output>> {
  template <typename In = Input, typename Out = Output>
  using ParserPtr = std::shared_ptr<Parser<In, Out>>;

public:
  using input_t = Input;
  using output_t = Output;
  using result_t = ParserResult<input_t, output_t>;
  using function_t = function_ptr<result_t, IStream<input_t>>;

  Parser(){};
  Parser(function_t func) : func(func) { assert(func && *func); };

  operator bool() const { return func && *func; }

  Parser &operator=(const Parser &parser) {
    assert(parser);
    func = parser.func;
    return *this;
  }

  ParserResult<Input, Output> operator()(IStream<Input> input) const {
    assert(*this);
    return std::invoke(*func, input);
  }

  template <typename R>
  ParserPtr<Input, R> operator>>=(function_ptr<ParserPtr<Input, R>, Output> f);

  template <typename R> ParserPtr<Input, R> as(R value);

  template <typename T> ParserPtr<Input, T> as() {
    return map<T>(make_function([](Output val) { return T{val}; }));
  }

  ParserPtr<Input, std::shared_ptr<Output>> shared() {
    return map<std::shared_ptr<Output>>(make_function(
        [](Output val) { return std::make_shared<Output>(val); }));
  }

  template <typename R> ParserPtr<Input, R> map(function_ptr<R, Output> f);

  ParserPtr<Input, Output> filter(function_ptr<bool, Output> f);

  template <typename R>
  ParserPtr<Input, Output> peek(ParserPtr<Input, R> parser);

  ParserPtr<Input, std::list<Output>> many();
  ParserPtr<Input, std::list<Output>> some();
  ParserPtr<Input, std::optional<Output>> maybe();

  ParserPtr<Input, Output> otherwise(Output output);

  function_t func;
};

template <typename Input, typename Output>
Parser(function_ptr<ParserResult<Input, Output>, IStream<Input>>)
    -> Parser<Input, Output>;

template <typename Input, typename Output>
using ParserPtr = std::shared_ptr<Parser<Input, Output>>;

template <typename Input, typename Output> auto make_parser() {
  auto x = std::make_shared<Parser<Input, Output>>();
  assert(x);
  return std::make_shared<Parser<Input, Output>>();
}

template <typename Input, typename Output>
auto make_parser(Parser<Input, Output> parser) {
  return std::make_shared<Parser<Input, Output>>(parser);
}

template <typename Input, typename Output>
auto make_parser(
    function_ptr<ParserResult<Input, Output>, IStream<Input>> function) {
  return std::make_shared<Parser<Input, Output>>(Parser(function));
}

template <typename Input, typename Output>
template <typename R>
inline auto
Parser<Input, Output>::operator>>=(function_ptr<ParserPtr<Input, R>, Output> f)
    -> ParserPtr<Input, R> {
  return make_parser(
      make_function([f, self = this->shared_from_this()](IStream<Input> input) {
        return std::invoke(*self->func, input) >>= [f](auto &&pair) {
          return std::invoke(std::invoke(*f, pair.first), pair.second);
        };
      }));
}

template <typename R, typename Input, typename... Outputs>
inline ParserPtr<Input, R> combine(function_ptr<R, Outputs...> f,
                                   ParserPtr<Input, Outputs>... args) {
  (..., assert(args));
  return make_parser(make_function([=](IStream<Input> input) {
    (..., assert(args));
    (..., assert(*args));
    return (Try(std::make_pair(std::tuple<>(), input)) >>= ... >>=
            [=](auto &&pair) {
              (..., assert(args));
              (..., assert(*args));

              return [=]() -> ParserResult<Input, Outputs> {
                try {
                  return std::invoke(*args, pair.second);
                } catch (const std::exception &e) {
                  return Error{e.what()};
                }
              }() >>= [=](auto &&p) {
                return Try(std::make_pair(
                    std::tuple_cat(pair.first, std::tuple(p.first)), p.second));
              };
            }) >>= [=](auto &&pair) {
      (..., assert(args));
      return Try(std::make_pair(std::apply(*f, pair.first), pair.second));
    };
  }));
}

template <size_t index, typename Input, typename... Args>
inline auto select(ParserPtr<Input, Args>... parser) -> ParserPtr<
    Input, typename std::tuple_element<index, std::tuple<Args...>>::type> {
  return combine(
      make_function([=](Args... args) { return pack_get<index>(args...); }),
      parser...);
}

template <typename Input, typename Output>
template <typename R>
inline ParserPtr<Input, R> Parser<Input, Output>::as(R value) {
  return combine(make_function([value](Output) { return value; }),
                 this->shared_from_this());
}

template <typename Input, typename Output>
template <typename R>
inline ParserPtr<Input, R>
Parser<Input, Output>::map(function_ptr<R, Output> f) {
  return combine(f, this->shared_from_this());
}

template <typename Input, typename Output>
inline ParserPtr<Input, Output>
Parser<Input, Output>::filter(function_ptr<bool, Output> f) {
  return make_parser(
      make_function([f, self = this->shared_from_this()](
                        IStream<Input> input) -> ParserResult<Input, Output> {
        assert(*self);
        auto res = std::invoke(*self->func, input);
        if (!res) {
          return res;
        }
        if (!std::invoke(*f, res->first)) {
          return Error{"filter fail."};
        }
        return res;
      }));
}

template <typename Input, typename Output>
template <typename R>
inline ParserPtr<Input, Output>
Parser<Input, Output>::peek(ParserPtr<Input, R> parser) {
  return make_parser(
      make_function([parser, self = this->shared_from_this()](
                        IStream<Input> input) -> ParserResult<Input, Output> {
        assert(*self);
        return std::invoke(*self->func, input) >>=
               [=](auto &&pair) -> ParserResult<Input, Output> {
          if (std::invoke(*parser, pair.second)) {
            return pair;
          } else {
            return Error{"peek fail."};
          }
        };
      }));
}

template <typename Input, typename Output>
inline ParserPtr<Input, std::list<Output>> Parser<Input, Output>::many() {
  return make_parser(make_function(
      [self = this->shared_from_this()](
          IStream<Input> input) -> ParserResult<Input, std::list<Output>> {
        assert(self);
        std::list<Output> output;
        result_t pair;
        while ((pair = std::invoke(*self->func, input))) {
          input = pair->second;
          output.push_back(pair->first);
        }
        return std::make_pair(output, input);
      }));
}

template <typename Input, typename Output>
inline ParserPtr<Input, std::list<Output>> Parser<Input, Output>::some() {
  return make_parser(make_function(
      [self = this->shared_from_this()](
          IStream<Input> input) -> ParserResult<Input, std::list<Output>> {
        assert(*self);
        std::list<Output> output;
        result_t pair;
        while ((pair = std::invoke(*self->func, input))) {
          input = pair->second;
          output.push_back(pair->first);
        }
        if (output.empty()) {
          return Error{"not find any match."};
        }
        return std::make_pair(output, input);
      }));
}

template <typename Input, typename Output>
inline ParserPtr<Input, std::optional<Output>> Parser<Input, Output>::maybe() {
  return make_parser(make_function(
      [self = this->shared_from_this()](
          IStream<Input> input) -> ParserResult<Input, std::optional<Output>> {
        assert(*self);
        std::optional<Output> output;
        result_t pair = std::invoke(*self->func, input);
        if (pair) {
          input = pair->second;
          output = pair->first;
        }
        return std::make_pair(output, input);
      }));
}

template <typename Input, typename Output>
inline ParserPtr<Input, Output> Parser<Input, Output>::otherwise(Output other) {
  return make_parser(
      make_function([other, self = this->shared_from_this()](
                        IStream<Input> input) -> ParserResult<Input, Output> {
        assert(*self);
        Output output = other;
        result_t pair = std::invoke(*self->func, input);
        if (pair) {
          input = pair->second;
          output = pair->first;
        }
        return std::make_pair(output, input);
      }));
}

template <typename Input, typename Output>
inline auto operator||(ParserPtr<Input, Output> lhs,
                       ParserPtr<Input, Output> rhs)
    -> ParserPtr<Input, Output> {
  return make_parser(
      make_function([=](IStream<Input> input) -> ParserResult<Input, Output> {
        return std::invoke(*lhs, input) || std::invoke(*rhs, input);
      }));
}

IStream<Char> &cinIStream();
} // namespace r5rs

#endif
