#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <iostream>
#include <sstream>
#include "../inc/types.h"
using namespace std;

struct AssemblyLineArguments;
struct InstructionArguments;
class Section;

namespace Utils {

  AssemblyLineArguments* create_arguments();
  InstructionArguments* create_instruction(int);
  int findBase(string value);
  void addWord(Section*& section, unsigned data, bool endianness);
  void toBytesHex(stringstream& ss, bool endianness);
}

#endif // UTILS_H_