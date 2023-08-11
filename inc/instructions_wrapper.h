#ifndef INSTRUCTIONS_WRAPPER_H_
#define INSTRUCTIONS_WRAPPER_H_

#include <iostream>
using namespace std;
struct AssemblyLineArguments;
struct InstructionArguments;

void instructionFirstPass();
void literalPool(InstructionArguments*);

void _asm_instr_halt();
void _asm_instr_int();
void _asm_instr_xchg(string*, string*);
void _asm_instr_arithmetic(int, string*, string*);
void _asm_instr_logical(int, string*, string*);
void _asm_instr_shl(string*, string*);
void _asm_instr_shr(string*, string*);
void _asm_instr_st(InstructionArguments*);
void _asm_instr_ld(InstructionArguments*);
void _asm_instr_ret(InstructionArguments*);
void _asm_instr_push(InstructionArguments*);
void _asm_instr_pop(InstructionArguments*);

#endif // INSTRUCTIONS_WRAPPER_H_