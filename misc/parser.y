%code requires{
#include <cstdio>
#include "inc/utils.hpp"
#include "inc/directives_wrapper.h"
#include "inc/instructions_wrapper.h"

extern int yylex();
extern int yyparse();
extern FILE* yyin;

extern int line_num;

void yyerror(const char* s);

}

%{ 
static bool secondPass = false;
%}

%output "parser.cpp"
%defines "parser.hpp"

%union {
  int ival;
  string* sval;
  AssemblyLineArguments* args;
  InstructionArguments* instr;
}

%token TOKEN_ENDL TOKEN_COMMA TOKEN_COLON TOKEN_PLUS TOKEN_MINUS TOKEN_PERCENT TOKEN_DOLLAR 
%token TOKEN_LEFT_SB TOKEN_RIGHT_SB
%token TOKEN_GLOBAL TOKEN_EXTERN TOKEN_SECTION TOKEN_WORD TOKEN_SKIP TOKEN_ASCII TOKEN_EQU TOKEN_END 
%token TOKEN_HALT TOKEN_INT TOKEN_RET TOKEN_IRET
%token TOKEN_POP TOKEN_PUSH TOKEN_CALL TOKEN_JMP TOKEN_XCHG
%token TOKEN_BEQ TOKEN_BNE TOKEN_BGT
%token TOKEN_ADD TOKEN_SUB TOKEN_MUL TOKEN_DIV
%token TOKEN_NOT TOKEN_AND TOKEN_OR TOKEN_XOR TOKEN_SHL TOKEN_SHR 
%token TOKEN_LD TOKEN_ST
%token TOKEN_CSRRD TOKEN_CSRWR

%token <sval> TOKEN_SYM
%token <sval> TOKEN_LITERAL
%token <sval> TOKEN_STRING
%token <sval> TOKEN_GPR

%type <args> symlist
%type <args> expr
%type <sval> register
%type <instr> operand

%%

input: 
  | line input
  ;

line:
  endls
  | directives
  | instructions
  ;
  

directives:
  TOKEN_SYM TOKEN_COLON TOKEN_GLOBAL symlist endls { _asm_label($1, secondPass); _asm_dir_global($4, secondPass); delete $1; delete $4; }
  | TOKEN_SYM TOKEN_COLON TOKEN_EXTERN symlist endls { _asm_label($1, secondPass); _asm_dir_extern($4, secondPass); delete $4; delete $1; }
  | TOKEN_SYM TOKEN_COLON TOKEN_SECTION TOKEN_SYM endls { _asm_label($1, secondPass); _asm_dir_section($4, secondPass); delete $1; delete $4; }
  | TOKEN_SYM TOKEN_COLON TOKEN_WORD symlist endls { _asm_label($1, secondPass); _asm_dir_word($4, secondPass); delete $1; delete $4; } 
  | TOKEN_SYM TOKEN_COLON TOKEN_SKIP TOKEN_LITERAL endls { _asm_label($1, secondPass); _asm_dir_skip($4, secondPass); delete $1; delete $4; }
  | TOKEN_SYM TOKEN_COLON TOKEN_ASCII TOKEN_STRING endls { _asm_label($1, secondPass); _asm_dir_ascii($4, secondPass); delete $1; delete $4; }
  | TOKEN_SYM TOKEN_COLON TOKEN_EQU TOKEN_SYM TOKEN_COMMA expr endls { cout << "Matched .equ directive with symbol preceding" << endl; }
  | TOKEN_SYM TOKEN_COLON TOKEN_END { _asm_label($1, secondPass); delete $1; _asm_dir_end(secondPass); return 15; }

  | TOKEN_GLOBAL symlist endls { _asm_dir_global($2, secondPass); delete $2; }
  | TOKEN_EXTERN symlist endls { _asm_dir_extern($2, secondPass); delete $2; } 
  | TOKEN_SECTION TOKEN_SYM endls { _asm_dir_section($2, secondPass); delete $2; }
  | TOKEN_WORD symlist endls { _asm_dir_word($2, secondPass); delete $2; }
  | TOKEN_SKIP TOKEN_LITERAL endls { _asm_dir_skip($2, secondPass); delete $2; }
  | TOKEN_ASCII TOKEN_STRING endls { _asm_dir_ascii($2, secondPass); delete $2; }
  | TOKEN_EQU TOKEN_SYM TOKEN_COMMA expr endls { cout << "Matched .equ directive" << endl; }
  | TOKEN_END { _asm_dir_end(secondPass); return 15; }
  | TOKEN_SYM TOKEN_COLON endls { _asm_label($1, secondPass); delete $1; }
  ;
  
  
instructions:
  TOKEN_SYM TOKEN_COLON TOKEN_HALT endls { if (secondPass) { _asm_instr_halt(); } else { _asm_label($1, secondPass); }
    instructionFirstPass(); 
   }
  | TOKEN_SYM TOKEN_COLON TOKEN_INT endls { if (secondPass) { _asm_instr_int(); } else { _asm_label($1, secondPass); }
    instructionFirstPass();
   }
  | TOKEN_SYM TOKEN_COLON TOKEN_IRET endls { cout << "iret instruction" << endl; }
  | TOKEN_SYM TOKEN_COLON TOKEN_RET endls { 
      if (secondPass) { 
        InstructionArguments* args = Utils::create_instruction(0x3); _asm_instr_ret(args); 
      } 
      instructionFirstPass();
    }
  | TOKEN_SYM TOKEN_COLON TOKEN_XCHG register TOKEN_COMMA register endls { 
    if (secondPass) { _asm_instr_xchg($4, $6); } else { _asm_label($1, secondPass); } instructionFirstPass();
  }
  | TOKEN_SYM TOKEN_COLON TOKEN_ADD register TOKEN_COMMA register endls {
    if(secondPass) { _asm_instr_arithmetic(0, $4, $6); } else { _asm_label($1, secondPass); } instructionFirstPass();  
  }
  | TOKEN_SYM TOKEN_COLON TOKEN_SUB register TOKEN_COMMA register endls {
    if(secondPass) { _asm_instr_arithmetic(1, $4, $6); } else { _asm_label($1, secondPass); } instructionFirstPass(); 
  }
  | TOKEN_SYM TOKEN_COLON TOKEN_MUL register TOKEN_COMMA register endls {
    if(secondPass) { _asm_instr_arithmetic(2, $4, $6); } else { _asm_label($1, secondPass); } instructionFirstPass();
  }
  | TOKEN_SYM TOKEN_COLON TOKEN_DIV register TOKEN_COMMA register endls {
    if(secondPass) { _asm_instr_arithmetic(3, $4, $6); } else { _asm_label($1, secondPass); } 
    instructionFirstPass();
  }
  | TOKEN_SYM TOKEN_COLON TOKEN_NOT register endls { if (secondPass) { 
      _asm_instr_logical(0, $4, $4); } else { _asm_label($1, secondPass); } instructionFirstPass();
    }
  | TOKEN_SYM TOKEN_COLON TOKEN_LD operand TOKEN_COMMA register endls { 
      if (secondPass) { $4->args->push_back({$6, 2}); _asm_instr_ld($4); } instructionFirstPass();
    }
  | TOKEN_SYM TOKEN_COLON TOKEN_PUSH operand endls {
      if (secondPass) { $4->modificator = 0x1; _asm_instr_push($4); } instructionFirstPass(); 
    }
  | TOKEN_SYM TOKEN_COLON TOKEN_POP operand endls {
      if (secondPass) { $4->modificator = 0x3; _asm_instr_pop($4); } instructionFirstPass(); 
    }

  | TOKEN_HALT endls { if (secondPass) { _asm_instr_halt(); } instructionFirstPass(); }
  | TOKEN_INT endls { if (secondPass) { _asm_instr_int(); }  instructionFirstPass(); }
  | TOKEN_IRET endls { cout << "iret" << endl; }
  | TOKEN_RET endls { 
      if (secondPass) { 
        InstructionArguments* args = Utils::create_instruction(0x3); _asm_instr_ret(args); 
      } 
      instructionFirstPass();  
    }
  | TOKEN_XCHG register TOKEN_COMMA register endls { if (secondPass) { _asm_instr_xchg($2, $4); } instructionFirstPass(); }
  | TOKEN_ADD register TOKEN_COMMA register endls { if(secondPass) { _asm_instr_arithmetic(0, $2, $4); }  instructionFirstPass(); }
  | TOKEN_SUB register TOKEN_COMMA register endls { if(secondPass) { _asm_instr_arithmetic(1, $2, $4); } instructionFirstPass(); }
  | TOKEN_MUL register TOKEN_COMMA register endls { if(secondPass) { _asm_instr_arithmetic(2, $2, $4); } instructionFirstPass(); }  
  | TOKEN_DIV register TOKEN_COMMA register endls { if(secondPass) { _asm_instr_arithmetic(3, $2, $4); } instructionFirstPass(); }
  | TOKEN_NOT register { if(secondPass) { _asm_instr_logical(0, $2, $2); } instructionFirstPass(); }
  | TOKEN_ST register TOKEN_COMMA operand endls { if (secondPass) { $4->args->push_back({$2, 2}); _asm_instr_st($4); } 
      if($4->modificator == 0x8 || $4->modificator == 0x9) { for (int i = 0; i < 4; i++) { instructionFirstPass(); } }
      else { instructionFirstPass(); }
    }
  | TOKEN_LD operand TOKEN_COMMA register endls { if (secondPass) { $2->args->push_back({$4, 2}); _asm_instr_ld($2); } 
      if ($2->modificator == 0x8 || $2->modificator == 0x9) { for (int i = 0; i < 4; i++) { instructionFirstPass(); } }
      else { instructionFirstPass(); }
    }
  | TOKEN_PUSH operand { if (secondPass) { $2->modificator=0x1; _asm_instr_push($2); } instructionFirstPass(); }
  | TOKEN_POP operand { if (secondPass) { $2->modificator = 0x3; _asm_instr_pop($2); } instructionFirstPass(); }
  ;

symlist:
  TOKEN_SYM { $$ = Utils::create_arguments(); $$->args->push_back({$1, 0}); }
  | TOKEN_LITERAL { $$ = Utils::create_arguments(); $$->args->push_back({$1, 1}); }
  | symlist TOKEN_COMMA TOKEN_LITERAL { $1->args->push_back({$3, 1}); $$ = $1; }
  | symlist TOKEN_COMMA TOKEN_SYM { $1->args->push_back({$3, 0}); $$ = $1; }
  ;

register: 
  TOKEN_PERCENT TOKEN_GPR { $$ = $2; }
  ;

operand:
  register { $$ = Utils::create_instruction(0x1); $$->args->push_back({$1, 2}); }
  | TOKEN_LEFT_SB register TOKEN_RIGHT_SB { $$ = Utils::create_instruction(0x2); $$->args->push_back({$2, 2}); }
  | TOKEN_DOLLAR TOKEN_LITERAL { $$ = Utils::create_instruction(0x2); $$->args->push_back({$2, 1}); if (!secondPass) { literalPool($$); } }
  | TOKEN_DOLLAR TOKEN_SYM { $$ = Utils::create_instruction(0x2); $$->args->push_back({$2, 0}); if (!secondPass) { literalPool($$); } }
  | TOKEN_LITERAL { $$ = Utils::create_instruction(0x8); $$->args->push_back({$1, 1}); if (!secondPass) { literalPool($$); } }
  | TOKEN_SYM { $$ = Utils::create_instruction(0x8); $$->args->push_back({$1, 0}); if (!secondPass) { literalPool($$); } } 
  | TOKEN_LEFT_SB register TOKEN_PLUS TOKEN_LITERAL TOKEN_RIGHT_SB { 
      $$ = Utils::create_instruction(0x9); 
      $$->args->push_back({$2, 2}); $$->args->push_back({$4, 1});
      if (!secondPass) { literalPool($$); }
    }
  | TOKEN_LEFT_SB register TOKEN_PLUS TOKEN_SYM TOKEN_RIGHT_SB { 
      $$ = Utils::create_instruction(0x9); 
      $$->args->push_back({$2, 2}); $$->args->push_back({$4, 0});
      if (!secondPass) { literalPool($$); }
    }
  ;

expr:
  TOKEN_SYM { $$ = Utils::create_arguments(); $$->args->push_back({$1, 0}); }
  | TOKEN_LITERAL { $$ = Utils::create_arguments(); $$->args->push_back({$1, 1}); }
  | expr TOKEN_PLUS TOKEN_LITERAL { 
      $1->args->push_back({new string("+"), 2});
      $1->args->push_back({$3, 1}); 
      $$ = $1; 
    }
  | expr TOKEN_PLUS TOKEN_SYM { 
      $1->args->push_back({new string("+"), 2});
      $1->args->push_back({$3, 0}); 
      $$ = $1; 
    }
  | expr TOKEN_MINUS TOKEN_LITERAL { 
      $1->args->push_back({new string("-"), 2});
      $1->args->push_back({$3, 1}); 
      $$ = $1; 
    }
  | expr TOKEN_MINUS TOKEN_SYM { 
      $1->args->push_back({new string("-"), 2});
      $1->args->push_back({$3, 0}); 
      $$ = $1; 
    }
  ;

endls: 
  endls TOKEN_ENDL
  | TOKEN_ENDL ;

%%

int main(int argc, char** argv) {
  FILE* fp = fopen("tests/test.s", "r");
  
  if (!fp) {
    cout << "File not found!" << endl;
    return -1;
  }

  yyin = fp;

  try {
    while(yyparse() != 15) { }
  }
  catch (const exception& e) {
    cout << e.what() << endl;
  }

  fseek(fp, 0, SEEK_SET);
  secondPass = !secondPass;

  try {
    while(yyparse() != 15) { }
  }
  catch (const exception& e) {
    cout << e.what() << endl;
  }

  return 0;
}

void yyerror(const char* s) {
  cout << "Parse error on line " << line_num << endl << "Message: " << s << endl;
  exit(-1);
}