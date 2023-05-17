#include "../inc/instructions_wrapper.h"
#include "../inc/Assembler.hpp"
#include "../inc/AssemblyInstructions.hpp"

void instructionFirstPass() {
  Assembler::getInstance().instructions().firstPass();
}

void _asm_instr_halt() {
  Assembler::getInstance().instructions().halt();
}

void _asm_instr_int() {
  Assembler::getInstance().instructions().interrupt();
}

void _asm_instr_xchg(string* regOne, string* regTwo) {
  Assembler::getInstance().instructions().xchg(*regOne, *regTwo);
}

void _asm_instr_arithmetic(int modificator, string* regOne, string* regTwo) {
  Assembler::getInstance().instructions().arithmetic(modificator, *regOne, *regTwo);
}

void _asm_instr_logical(int modificator, string* regOne, string* regTwo) {
  Assembler::getInstance().instructions().logical(modificator, *regOne, *regTwo);
}