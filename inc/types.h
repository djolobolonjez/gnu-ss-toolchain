#ifndef TYPES_H_
#define TYPES_H_

#include <vector>
#include <iostream>
#include <string>
using namespace std;

typedef vector<string*> ArgVector;

typedef unsigned long ulong;
typedef unsigned char byte;

struct AssemblyLineArguments {
  ArgVector* args;

  AssemblyLineArguments() {
    args = new ArgVector();
  }
};

struct InstructionArguments : public AssemblyLineArguments {

  InstructionArguments() {
    
  }
};

struct SymbolTableEntry {
    ulong value;
    ulong size;
    byte info;
    int index;
    string name;
    bool defined = false;
    bool ext = false;

    SymbolTableEntry(ulong _value, string _name, 
                     byte _info, int _index, 
                     ulong _size
                     ) : value(_value), name(_name), index(_index), info(_info), size(_size) {}

    bool isDefined() const { return defined; }
    bool isExtern() const { return ext; }
    string getName() const { return name; }

    void setDefined(bool _defined) { defined = _defined; }
    void setExtern(bool _ext) { ext = _ext; }
    void setIndex(int _index) { index = _index; } 
    void setInfo(byte _info) { info = _info; }
    void setValue(ulong _value) { value = _value; }
  };

#endif // TYPES_H_