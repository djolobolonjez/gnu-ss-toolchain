%{
#include "parser.hpp"
#include <iostream>
#include <string>
using namespace std;
struct AssemblyLineArguments;
struct arg;
struct InstructionArguments;

int line_num = 1;
%}

%option outfile="lexer.cpp" header-file="lexer.hpp"

%option noyywrap 

%%
[ \t\r]+
"#"[^\n\r]* 
, { return TOKEN_COMMA; }
":" { return TOKEN_COLON; }
"+" { return TOKEN_PLUS; }
"-" { return TOKEN_MINUS; }
"%" { return TOKEN_PERCENT; }
"$" { return TOKEN_DOLLAR; }
"[" { return TOKEN_LEFT_SB; }
"]" { return TOKEN_RIGHT_SB; }
\.global { return TOKEN_GLOBAL; }
\.extern { return TOKEN_EXTERN; }
\.section { return TOKEN_SECTION; }
\.word { return TOKEN_WORD; }
\.skip { return TOKEN_SKIP; }
\.ascii { return TOKEN_ASCII; }
\.equ { return TOKEN_EQU; }
\.end { return TOKEN_END; }
halt { return TOKEN_HALT; }
int { return TOKEN_INT; }
iret { return TOKEN_IRET; }
ret { return TOKEN_RET; }
call { return TOKEN_CALL; }
jmp { return TOKEN_JMP; }
beq { return TOKEN_BEQ; }
bne { return TOKEN_BNE; }
bgt { return TOKEN_BGT; }
push { return TOKEN_PUSH; }
pop { return TOKEN_POP; }
xchg { return TOKEN_XCHG; }
add { return TOKEN_ADD; }
sub { return TOKEN_SUB; }
mul { return TOKEN_MUL; }
div { return TOKEN_DIV; }
not { return TOKEN_NOT; }
and { return TOKEN_AND; }
or { return TOKEN_OR; }
xor { return TOKEN_XOR; }
shl { return TOKEN_SHL; }
shr { return TOKEN_SHR; }
ld { return TOKEN_LD; }
st { return TOKEN_ST; }
csrrd { return TOKEN_CSRRD; }
csrwr { return TOKEN_CSRWR; }
(([rR]([0-9]|1[0-5]))|([sS][pP])|([pP][cC])) { yylval.sval = new string(yytext); return TOKEN_GPR; }
((status)|(handler)|(cause)) { yylval.sval = new string(yytext); return TOKEN_CSR; }
(([1-9][0-9]*)|(0x[0-9a-fA-F]+)|(0[0-7]+)) { yylval.sval = new string(yytext); return TOKEN_LITERAL; }
\".*\" { yylval.sval = new string(yytext); return TOKEN_STRING; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.sval = new string(yytext); return TOKEN_SYM; }
\n { ++line_num; return TOKEN_ENDL; }
. { }

%%