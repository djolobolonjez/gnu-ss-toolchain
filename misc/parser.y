%code requires{
#include <cstdio>
#include "inc/types.h"
#include "inc/utils.h"
#include "inc/directives_wrapper.h"

extern int yylex();
extern int yyparse();
extern FILE* yyin;

extern int line_num;

void yyerror(const char* s);

}

%{ 
%}

%output "parser.cpp"
%defines "parser.hpp"

%union {
  int ival;
  string* sval;
  AssemblyLineArguments* args;
}

%token TOKEN_ENDL TOKEN_COMMA TOKEN_COLON
%token TOKEN_GLOBAL TOKEN_EXTERN TOKEN_SECTION TOKEN_END 

%token <sval> TOKEN_SYM

%type <args> symlist

%%

input: 
  | line input
  ;

line:
  endls
  | TOKEN_SYM TOKEN_COLON TOKEN_GLOBAL symlist endls { 
      if (_asm_label($1) < 0) { delete $4; delete $1; return -1; } _asm_dir_global($4); delete $1; delete $4; 
    }
  | TOKEN_GLOBAL symlist endls { _asm_dir_global($2); delete $2; }
  | TOKEN_EXTERN symlist endls { 
      if (_asm_dir_extern($2) < 0) { delete $2; return -1; } delete $2;
    }
  | TOKEN_SYM TOKEN_COLON TOKEN_EXTERN symlist endls { 
      if (_asm_label($1) < 0) { delete $4; delete $1; return -1; } 
      if (_asm_dir_extern($4) < 0) { delete $1; delete $4; return -1; } delete $1; delete $4; 
    }
  | TOKEN_SYM TOKEN_COLON TOKEN_SECTION TOKEN_SYM endls {
      if (_asm_label($1) < 0) { delete $1; delete $4; return -1; } _asm_dir_section($4); delete $1; delete $4;
    }
  | TOKEN_SECTION TOKEN_SYM endls { _asm_dir_section($2); delete $2; }
  | TOKEN_SYM TOKEN_COLON endls { if (_asm_label($1) < 0) { delete $1; return -1; } delete $1; }
  | TOKEN_SYM TOKEN_COLON TOKEN_END { 
      if (_asm_label($1) < 0) { delete $1; return -1; } _asm_dir_end(); return 15;
    }
  | TOKEN_END { _asm_dir_end(); return 15; }
  ;

symlist:
  TOKEN_SYM { $$ = create_arguments(); $$->args->push_back($1); }
  | symlist TOKEN_COMMA TOKEN_SYM { $1->args->push_back($3); $$ = $1; }
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

  while (1) {
    int ret = yyparse();
    if (ret == -1 || ret == 15) {
      break;
    }
  }

  return 0;
}

void yyerror(const char* s) {
  cout << "Parse error on line " << line_num << endl << "Message: " << s << endl;
  exit(-1);
}