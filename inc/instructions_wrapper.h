#ifndef INSTRUCTIONS_WRAPPER_H_
#define INSTRUCTIONS_WRAPPER_H_

#include <iostream>
using namespace std;
struct AssemblyLineArguments;
struct InstructionArguments;

void instructionFirstPass();

void _asm_instr_halt();
void _asm_instr_int();
void _asm_instr_xchg(string*, string*);
void _asm_instr_arithmetic(int, string*, string*);
void _asm_instr_logical(int, string*, string*);
void _asm_instr_shl(string*, string*);
void _asm_instr_shr(string*, string*);
void _asm_instr_ld(InstructionArguments*);

#endif // INSTRUCTIONS_WRAPPER_H_