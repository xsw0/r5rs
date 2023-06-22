#ifndef _R5RS_LEX_H_
#define _R5RS_LEX_H_

#include "Parse.h"
#include "Token.h"

#include <unordered_map>

namespace r5rs
{
  IStream<Token> tokens(IStream<Char> input);

  namespace lex
  {
    function_ptr<Char, char> char2Char();
    function_ptr<std::string, IStream<char>> charStream2String();
    function_ptr<std::string, std::list<char>> charList2String();

    Parser<Char, char> any();

    Parser<Char, char> match(char c);
    Parser<Char, char> match(function_ptr<bool, char> pred);
    Parser<Char, char> match_any_of(std::string set);

    Parser<Char, std::string> match(std::string str);

    Parser<Char, nullptr_t> delimiter();
    Parser<Char, std::string> comment();
    Parser<Char, nullptr_t> intertoken_space();

    Parser<Char, std::string> identifier();
    Parser<Char, char> initial();
    Parser<Char, char> letter();
    Parser<Char, char> special_initial();
    Parser<Char, char> subsequent();
    Parser<Char, char> digit();
    Parser<Char, char> special_subsequent();
    Parser<Char, std::string> peculiar_identifier();

    Parser<Char, bool> boolean();
    Parser<Char, char> character();
    Parser<Char, char> character_name();

    Parser<Char, std::string> string();
    Parser<Char, char> string_element();
    Parser<Char, int64_t> number();

    Parser<Char, TokenType> single_symbol();
    Parser<Char, TokenType> two_char_symbol();
    Parser<Char, TokenType> symbol();
    Parser<Char, Token> token();

    Parser<Char, Token> eof();
  } // namespace lex
} // namespace r5rs

#endif
