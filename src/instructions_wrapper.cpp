#include "../inc/instructions_wrapper.h"
#include "../inc/Assembler.hpp"
#include "../inc/AssemblyInstructions.hpp"
#include "../inc/types.h"

void literalPool(InstructionArguments* instruction) {
  Assembler::getInstance().instructions().literalPool(instruction);
}

void instructionFirstPass(InstructionArguments* instruction) {
  Assembler::getInstance().instructions().firstPass(instruction);
}

void _asm_instr_halt() {
  Assembler::getInstance().instructions().halt();
}

void _asm_instr_int() {
  Assembler::getInstance().instructions().interrupt();
}

void _asm_instr_iret() {
  Assembler::getInstance().instructions().iret();
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

void _asm_instr_shl(string* regOne, string* regTwo) {
  Assembler::getInstance().instructions().shl(*regOne, *regTwo);
}

void _asm_instr_shr(string* regOne, string* regTwo) {
  Assembler::getInstance().instructions().shr(*regOne, *regTwo);
}

void _asm_instr_st(InstructionArguments* args) {
  if (args->modificator == 0x1) {
    args->modificator = 0x3;
  }
  else if (args->modificator == 0x2 && (*(args->args))[0].type == 2) {
    args->modificator = 0x0;
  }
  Assembler::getInstance().instructions().st(args, { });
}

void _asm_instr_ld(InstructionArguments* args) {
  Assembler::getInstance().instructions().ld(args, { });
}

void _asm_instr_ret(InstructionArguments* args) {
  Assembler::getInstance().instructions().ret(args);
}

void _asm_instr_push(InstructionArguments* args) {
  Assembler::getInstance().instructions().push(args);
}

void _asm_instr_pop(InstructionArguments* args) {
  Assembler::getInstance().instructions().pop(args);
}

void _asm_instr_call(InstructionArguments* args) {
  Assembler::getInstance().instructions().call(args);
}

void _asm_instr_jmp(InstructionArguments* args) {
  Assembler::getInstance().instructions().jmp(args);
}

void _asm_instr_csrrd(InstructionArguments* args) {
  Assembler::getInstance().instructions().csrrd(args);
}

void _asm_instr_csrwr(InstructionArguments* args) {
  Assembler::getInstance().instructions().csrwr(args);
}