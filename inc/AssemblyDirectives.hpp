#ifndef ASSEMBLY_DIRECTIVES_HPP_
#define ASSEMBLY_DIRECTIVES_HPP_

#include <iostream>
using namespace std;

struct AssemblyLineArguments;

class AssemblyDirectives {
private:
  AssemblyDirectives() = default;

  void checkNoSection();
public:
  static AssemblyDirectives& getInstance();

  void global(AssemblyLineArguments*);
  void ext(AssemblyLineArguments*);
  void section(string*);
  void ascii(string);
  void word(AssemblyLineArguments*);
  void skip(string, int);
  void end();
};

#endif // ASSEMBLY_DIRECTIVES_HPP_