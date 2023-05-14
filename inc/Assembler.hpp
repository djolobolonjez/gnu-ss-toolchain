#ifndef ASSEMBLER_HPP_
#define ASSEMBLER_HPP_

#include <iostream>
using namespace std;

class AssemblyDirectives;

class Assembler {

private:
  Assembler() = default;
public:
  static Assembler& getInstance();
  AssemblyDirectives& directives(); 
  
  int label(string*);
  void printSymbolTable();
};

#endif // ASSEMBLER_HPP_