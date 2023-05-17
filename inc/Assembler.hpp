#ifndef ASSEMBLER_HPP_
#define ASSEMBLER_HPP_

#include <iostream>
using namespace std;

class AssemblyDirectives;
class AssemblyInstructions;

class Assembler {

private:
  Assembler() = default;
  bool firstPass = true;
  void secondPass() { firstPass = !firstPass; }
public:
  static Assembler& getInstance();
  AssemblyDirectives& directives(); 
  AssemblyInstructions& instructions();

  bool isFirstPass() const { return firstPass; }

  void sectionReset();

  int label(string*);
  void printSymbolTable();
};

#endif // ASSEMBLER_HPP_