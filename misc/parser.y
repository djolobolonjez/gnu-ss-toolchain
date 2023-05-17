%code requires{
#include <cstdio>
#include "inc/utils.hpp"
#include "inc/directives_wrapper.h"

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

%type <args> symlist
%type <args> expr

%%

input: 
  | line input
  ;

line:
  endls
  | TOKEN_SYM TOKEN_COLON TOKEN_GLOBAL symlist endls { _asm_label($1, secondPass); _asm_dir_global($4, secondPass); delete $1; delete $4; }
  | TOKEN_GLOBAL symlist endls { _asm_dir_global($2, secondPass); delete $2; }

  | TOKEN_SYM TOKEN_COLON TOKEN_EXTERN symlist endls { _asm_label($1, secondPass); _asm_dir_extern($4, secondPass); delete $4; delete $1; }
  | TOKEN_EXTERN symlist endls { _asm_dir_extern($2, secondPass); delete $2; } 

  | TOKEN_SYM TOKEN_COLON TOKEN_SECTION TOKEN_SYM endls { _asm_label($1, secondPass); _asm_dir_section($4, secondPass); delete $1; delete $4; }
  | TOKEN_SECTION TOKEN_SYM endls { _asm_dir_section($2, secondPass); delete $2; }

  | TOKEN_SYM TOKEN_COLON endls { _asm_label($1, secondPass); delete $1; }

  | TOKEN_SYM TOKEN_COLON TOKEN_WORD symlist endls { _asm_label($1, secondPass); _asm_dir_word($4, secondPass); delete $1; delete $4; } 
  | TOKEN_WORD symlist endls { _asm_dir_word($2, secondPass); delete $2; }

  | TOKEN_SYM TOKEN_COLON TOKEN_SKIP TOKEN_LITERAL endls { _asm_label($1, secondPass); _asm_dir_skip($4, secondPass); delete $1; delete $4; }
  | TOKEN_SKIP TOKEN_LITERAL endls { _asm_dir_skip($2, secondPass); delete $2; }

  | TOKEN_SYM TOKEN_COLON TOKEN_ASCII TOKEN_STRING endls { _asm_label($1, secondPass); _asm_dir_ascii($4, secondPass); delete $1; delete $4; }
  | TOKEN_ASCII TOKEN_STRING endls { _asm_dir_ascii($2, secondPass); delete $2; }

  | TOKEN_SYM TOKEN_COLON TOKEN_EQU TOKEN_SYM TOKEN_COMMA expr endls { cout << "Matched .equ directive with symbol preceding" << endl; }
  | TOKEN_EQU TOKEN_SYM TOKEN_COMMA expr endls { cout << "Matched .equ directive" << endl; }

  | TOKEN_SYM TOKEN_COLON TOKEN_END { _asm_label($1, secondPass); delete $1; _asm_dir_end(secondPass); return 15; }
  | TOKEN_END { _asm_dir_end(secondPass); return 15; }

  | TOKEN_SYM TOKEN_COLON TOKEN_HALT endls { cout << "halt instruction" << endl; }
  | TOKEN_HALT endls { cout << "halt" << endl; }
  
  | TOKEN_SYM TOKEN_COLON TOKEN_INT endls { cout << "int instruction" << endl; }
  | TOKEN_INT endls { cout << "int" << endl; }

  | TOKEN_SYM TOKEN_COLON TOKEN_IRET endls { cout << "iret instruction" << endl; }
  | TOKEN_IRET endls { cout << "iret" << endl; }

  | TOKEN_SYM TOKEN_COLON TOKEN_RET endls { cout << "ret instruction" << endl; }
  | TOKEN_RET endls { cout << "ret" << endl; }
  ;

symlist:
  TOKEN_SYM { $$ = Utils::create_arguments(); $$->args->push_back({$1, 0}); }
  | TOKEN_LITERAL { $$ = Utils::create_arguments(); $$->args->push_back({$1, 1}); }
  | symlist TOKEN_COMMA TOKEN_LITERAL { $1->args->push_back({$3, 1}); $$ = $1; }
  | symlist TOKEN_COMMA TOKEN_SYM { $1->args->push_back({$3, 0}); $$ = $1; }
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
  secondPass = true;

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