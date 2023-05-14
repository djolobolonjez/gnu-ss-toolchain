#ifndef ASSEMBLY_DIRECTIVES_HPP_
#define ASSEMBLY_DIRECTIVES_HPP_

#include <iostream>
using namespace std;

struct AssemblyLineArguments;

class AssemblyDirectives {
private:
  AssemblyDirectives() = default;
public:
  static AssemblyDirectives& getInstance();

  void global(AssemblyLineArguments*);
  int ext(AssemblyLineArguments*);
  void section(string*);
  void end();
};

#endif // ASSEMBLY_DIRECTIVES_HPP_