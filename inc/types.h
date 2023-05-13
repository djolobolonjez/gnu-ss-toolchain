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

struct SymbolTableEntry {
    ulong value;
    ulong size;
    byte info;
    int index;
    string name;

    SymbolTableEntry(ulong _value, string _name, 
                     byte _info, int _index, 
                     ulong _size
                     ) : value(_value), name(_name), index(_index), info(_info), size(_size) {}
  };

#endif // TYPES_H_