#ifndef DIRECTIVES_WRAPPER_H
#define DIRECTIVES_WRAPPER_H

#include <iostream>
using namespace std;
struct AssemblyLineArguments;

void _asm_dir_global(AssemblyLineArguments*);
void _asm_dir_extern(AssemblyLineArguments*);
void _asm_dir_section(string*);
void _asm_dir_word(AssemblyLineArguments*);
void _asm_dir_skip(string*);
void _asm_dir_ascii(string*);
void _asm_label(string*);
void _asm_dir_end();

#endif // DIRECTIVES_WRAPPER_H