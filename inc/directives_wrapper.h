#ifndef DIRECTIVES_WRAPPER_H
#define DIRECTIVES_WRAPPER_H

#include <iostream>
using namespace std;
struct AssemblyLineArguments;

void _asm_dir_global(AssemblyLineArguments*, bool);
void _asm_dir_extern(AssemblyLineArguments*, bool);
void _asm_dir_section(string*, bool);
void _asm_dir_word(AssemblyLineArguments*, bool);
void _asm_dir_skip(string*, bool);
void _asm_dir_ascii(string*, bool);
void _asm_label(string*, bool);
void _asm_dir_end(bool);

#endif // DIRECTIVES_WRAPPER_H