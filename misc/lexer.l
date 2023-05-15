%{
#include "parser.hpp"
#include <iostream>
#include <string>
using namespace std;
struct AssemblyLineArguments;
struct arg;

int line_num = 1;
%}

%option outfile="lexer.cpp" header-file="lexer.hpp"

%option noyywrap 

%%
[ \t\r]+
"#"[^\n\r]* 
, { return TOKEN_COMMA; }
":" { return TOKEN_COLON; }
\.global { return TOKEN_GLOBAL; }
\.extern { return TOKEN_EXTERN; }
\.section { return TOKEN_SECTION; }
\.word { return TOKEN_WORD; }
\.skip { return TOKEN_SKIP; }
\.ascii { return TOKEN_ASCII; }
\.end { return TOKEN_END; }
(([1-9][0-9]*)|(0x[0-9a-fA-F]+)|(0[0-7]+)) { yylval.sval = new string(yytext); return TOKEN_LITERAL; }
\".*\" { yylval.sval = new string(yytext); return TOKEN_STRING; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.sval = new string(yytext); return TOKEN_SYM; }
\n { ++line_num; return TOKEN_ENDL; }
. { }

%%