#ifndef DIRECTIVES_WRAPPER_H
#define DIRECTIVES_WRAPPER_H

#include <iostream>
using namespace std;
struct AssemblyLineArguments;

void _asm_dir_global(AssemblyLineArguments*);
int _asm_dir_extern(AssemblyLineArguments*);
void _asm_dir_section(string*);
int _asm_label(string*);
void _asm_dir_end();

#endif // DIRECTIVES_WRAPPER_H