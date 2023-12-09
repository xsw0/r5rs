#ifndef R5RS_LEX_H
#define R5RS_LEX_H

#include "Parse.h"
#include "Token.h"

#include <unordered_map>

namespace r5rs {
  IStream<Token> tokens(IStream<Char> input);

  namespace lex {
    function_ptr<Char, char> char2Char();
    function_ptr<std::string, IStream<char>> charStream2String();
    function_ptr<std::string, std::list<char>> charList2String();

    ParserPtr<Char, char> any();

    ParserPtr<Char, char> match(char c);
    ParserPtr<Char, char> match(function_ptr<bool, char> pred);
    ParserPtr<Char, char> match_any_of(std::string set);

    ParserPtr<Char, std::string> match(std::string str);

    ParserPtr<Char, nullptr_t> delimiter();
    ParserPtr<Char, std::string> comment();
    ParserPtr<Char, nullptr_t> intertoken_space();

    ParserPtr<Char, std::string> identifier();
    ParserPtr<Char, char> initial();
    ParserPtr<Char, char> letter();
    ParserPtr<Char, char> special_initial();
    ParserPtr<Char, char> subsequent();
    ParserPtr<Char, char> digit();
    ParserPtr<Char, char> special_subsequent();
    ParserPtr<Char, std::string> peculiar_identifier();

    ParserPtr<Char, bool> boolean();
    ParserPtr<Char, char> character();
    ParserPtr<Char, char> character_name();

    ParserPtr<Char, std::string> string();
    ParserPtr<Char, char> string_element();
    ParserPtr<Char, int64_t> number();

    ParserPtr<Char, TokenType> single_symbol();
    ParserPtr<Char, TokenType> two_char_symbol();
    ParserPtr<Char, TokenType> symbol();
    ParserPtr<Char, Token> token();

    ParserPtr<Char, Token> eof();
  } // namespace lex
} // namespace r5rs

#endif
