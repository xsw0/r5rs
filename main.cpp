#include <iostream>
#include <fstream>

#include "GC.h"
#include "Lex.h"

using namespace std;
using namespace r5rs;

bool is_digit(char c)
{
  return std::isdigit(c);
}

int main(int argc, char * argv[])
{
  auto add = malloc(sizeof(std::vector<Reference>));

  if (argc == 1)
  {
    std::string line;
    while (std::getline(std::cin, line))
    {
      CharInput input{ line.cbegin(), line.cend() };

      auto lexer = token_character().many();
      auto res = lexer(input)->first;
      for (auto && c : res)
      {
        cout << (int)c;
      }
      cout << endl;
    }
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
