#include <iostream>

#include "Type.h"
#include "GC.h"
#include "Token.h"
#include "Lex.h"
#include "Parse.h"
#include "String.h"

using namespace r5rs;

int main(int argc, char * argv[])
{
  auto toks = tokens(cinIStream());

  for (size_t i = 0; toks[i]; ++i)
  {
    auto && tok = *toks[i];
    std::cout << TokenType2String(tok.type) << std::endl
      << std::visit(String(), tok.value) << std::endl;
  }

  return 0;
}
