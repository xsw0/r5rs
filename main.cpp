#include <iostream>
#include <fstream>

#include "GC.h"
#include "Lex.h"
#include "Expressions.h"
#include "color.h"
#include "String.h"
#include "Interpreter.h"

using namespace std;
using namespace r5rs;

int main(int argc, char * argv[])
{
  if (argc == 1)
  {
    // r5rs::expression::init();

    std::cout << "start" << std::endl;

    auto toks = tokens(cinIStream());

    // for (size_t i = 0; toks[i]; ++i)
    // {
    //   auto && tok = *toks[i];
    //   std::cout
    //     << C_BOLD << r5rs::to_string(tok.type)
    //     << C_BLACK << "[" << tok.row << ":" << tok.col << "]:"
    //     << C_GREEN << std::visit(String(), tok.value)
    //     << C_BLACK << std::endl;
    // }

    auto stream = ast(toks);

    Interpreter interpreter;

    Try<r5rs::expression::CODPtr> cod;

    std::string mem_dot = "../dot/mem.dot";
    std::ofstream file(mem_dot, std::ios::trunc | std::ios::out);
    file << interpreter.env->print() << std::endl;

    while ((cod = stream[0]))
    {
      std::cout << std::visit(String(), *std::invoke(interpreter, cod->get())) << std::endl;
      std::ofstream file(mem_dot, std::ios::trunc | std::ios::out);
      file << interpreter.env->print() << std::endl;
      stream += 1;
    }

    std::cout << "end" << std::endl;
  }
  else if (argc == 2)
  {

  }
  else
  {
    std::cerr << "usage: r5rs [filename]" << std::endl;
    return -1;
  }
  return 0;
}
