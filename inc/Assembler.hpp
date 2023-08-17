#ifndef ASSEMBLER_HPP_
#define ASSEMBLER_HPP_

#include <iostream>
using namespace std;

class AssemblyDirectives;
class AssemblyInstructions;
class Section; 

class Assembler {

private:
  Assembler() = default;
  bool firstPass = true;
  string filename;
  void secondPass() { firstPass = !firstPass; }
public:
  static Assembler& getInstance();
  AssemblyDirectives& directives(); 
  AssemblyInstructions& instructions();

  void dumpSectionData(Section*&, ofstream&);
  void dumpSymbolTable(ofstream&);
  void setFileName(string filename) { this->filename = filename; }
  bool isFirstPass() const { return firstPass; }

  void sectionReset();

  string getFileName() const { return filename; }

  int label(string*);
  void printSymbolTable();
};

#endif // ASSEMBLER_HPP_