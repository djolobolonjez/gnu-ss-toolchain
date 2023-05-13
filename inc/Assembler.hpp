#ifndef ASSEMBLER_HPP_
#define ASSEMBLER_HPP_

class AssemblyDirectives;

class Assembler {

private:
  Assembler() = default;
public:
  static Assembler& getInstance();
  AssemblyDirectives& directives(); 

  void printSymbolTable();
};

#endif // ASSEMBLER_HPP_