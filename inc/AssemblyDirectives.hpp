#ifndef ASSEMBLY_DIRECTIVES_HPP_
#define ASSEMBLY_DIRECTIVES_HPP_

#include <iostream>
using namespace std;

struct AssemblyLineArguments;
class Section;

class AssemblyDirectives {
private:
  AssemblyDirectives() = default;

  void checkNoSection();
  
  void addWord(Section*&, int);
  void wordLiteral(string value);
  void wordSymbol(string symbol); 
public:
  static AssemblyDirectives& getInstance();

  void global(AssemblyLineArguments*);
  void ext(AssemblyLineArguments*);
  void sectionPassOne(string*);
  void sectionPassTwo(string*);
  void asciiPassOne(string);
  void asciiPassTwo(string);
  void wordPassOne(AssemblyLineArguments*);
  void wordPassTwo(AssemblyLineArguments*);
  void skipPassOne(string, int);
  void skipPassTwo(string, int);
  void end();
};

#endif // ASSEMBLY_DIRECTIVES_HPP_