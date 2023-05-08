#ifndef _R5RS_TOKEN_H_
#define _R5RS_TOKEN_H_

#include <string>

#define R5RS_TOKEN_TYPE                                     \
  R5RS_TOKEN_TYPE_ACCESS(err)                               \
                                                            \
  R5RS_TOKEN_TYPE_ACCESS(variable)                          \
  R5RS_TOKEN_TYPE_ACCESS(left_paren)                        \
  R5RS_TOKEN_TYPE_ACCESS(right_paren)                       \
  R5RS_TOKEN_TYPE_ACCESS(vector_paren)            /* #( */  \
  R5RS_TOKEN_TYPE_ACCESS(quote_symbol)            /* '  */  \
  R5RS_TOKEN_TYPE_ACCESS(quasiquote_symbol)       /* `  */  \
  R5RS_TOKEN_TYPE_ACCESS(unquote_symbol)          /* ,  */  \
  R5RS_TOKEN_TYPE_ACCESS(unquote_splicing_symbol) /* ,@ */  \
  R5RS_TOKEN_TYPE_ACCESS(dot)                     /* .  */  \
                                                            \
  R5RS_TOKEN_TYPE_ACCESS(boolean)                           \
  R5RS_TOKEN_TYPE_ACCESS(character)                         \
  R5RS_TOKEN_TYPE_ACCESS(string)                            \
  R5RS_TOKEN_TYPE_ACCESS(number)                            \
                                                            \
  /* syntactic keyword */                                   \
  R5RS_TOKEN_TYPE_ACCESS(Else)                              \
  R5RS_TOKEN_TYPE_ACCESS(evaluates_to) /* => */             \
  R5RS_TOKEN_TYPE_ACCESS(define)                            \
                                                            \
  R5RS_TOKEN_TYPE_ACCESS(unquote)                           \
  R5RS_TOKEN_TYPE_ACCESS(unquote_splicing)                  \
                                                            \
  /* expression keyword */                                  \
  R5RS_TOKEN_TYPE_ACCESS(quote)                             \
  R5RS_TOKEN_TYPE_ACCESS(lambda)                            \
  R5RS_TOKEN_TYPE_ACCESS(If)                                \
                                                            \
  R5RS_TOKEN_TYPE_ACCESS(set_)                              \
  R5RS_TOKEN_TYPE_ACCESS(begin)                             \
  R5RS_TOKEN_TYPE_ACCESS(cond)                              \
  R5RS_TOKEN_TYPE_ACCESS(And)                               \
  R5RS_TOKEN_TYPE_ACCESS(Or)                                \
  R5RS_TOKEN_TYPE_ACCESS(Case)                              \
                                                            \
  R5RS_TOKEN_TYPE_ACCESS(let)                               \
  R5RS_TOKEN_TYPE_ACCESS(let_)                              \
  R5RS_TOKEN_TYPE_ACCESS(letrec)                            \
  R5RS_TOKEN_TYPE_ACCESS(Do)                                \
  R5RS_TOKEN_TYPE_ACCESS(delay)                             \
                                                            \
  R5RS_TOKEN_TYPE_ACCESS(quasiquote)                        \
                                                            \
  R5RS_TOKEN_TYPE_ACCESS(eof)                               \


namespace r5rs
{
  enum class TokenType
  {
#define R5RS_TOKEN_TYPE_ACCESS(id) id,
    R5RS_TOKEN_TYPE
#undef R5RS_TOKEN_TYPE_ACCESS
  };

  std::string TokenType2String(TokenType type);

  class Token final
  {
  public:
    using value_t = std::variant<
      nullptr_t,
      bool,
      char,
      int64_t,
      std::string
    >;
    TokenType type = TokenType::err;
    value_t value = nullptr;
    size_t row = 0;
    size_t col = 0;
  };
} // namespace r5rs

#endif
