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
  void addWord(Section*& section, unsigned data, bool endianness, int container = 0);
  void toBytesHex(stringstream& ss, bool endianness);
  void editContent(vector<char>& content, unsigned data, bool endianness, int offset);
}

#endif // UTILS_H_