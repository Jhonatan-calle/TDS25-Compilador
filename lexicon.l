%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include "syntax.tab.h"

  void yyerror(char* msg);
%}

%option noyywrap
%option yylineno

letter                [a-zA-Z]
number                [0-9]+
literals              [-]?{number}
ID 			              {letter}({letter}|{number}|_)*
endOfStatement        ;
operator              [+*<]
delimiter             [(){}]

/* reserved keywords */
extern                extern
boolType              bool
intType               integer
voidType              void
returnStatement       return
whileLoop             while
beginProgram          program
ifStatement           if
thenStatement         then
elseStatement         else
trueValue             true
falseValue            false

/* regex of ignores */
commentUniline        "//"[^\n]*
commentMultiline      "/*"(.|\n)*"*/"
spacesNewlinesTabs    [ \t\r\n]+
ignoredRegex          {commentUniline}|{commentMultiline}|{spacesNewlinesTabs}


%%

{endOfStatement}        { return T_EOS; }

{literals}              { yylval.num = atoi(yytext); return V_NUM; }
{trueValue}             { yylval.num = 1; return V_TRUE; }
{falseValue}            { yylval.num = 0; return V_FALSE; }

{extern}                { yylval.str = strdup(yytext); return R_EXTERN; }
{boolType}              { yylval.str = strdup(yytext); return R_BOOL; }
{intType}               { yylval.str = strdup(yytext); return R_INT; }
{voidType}              { yylval.str = strdup(yytext); return R_VOID; }
{returnStatement}       { yylval.str = strdup(yytext); return R_RETURN; }
{whileLoop}             { yylval.str = strdup(yytext); return R_WHILE; }
{beginProgram}          { yylval.str = strdup(yytext); return R_PROGRAM; }
{ifStatement}           { yylval.str = strdup(yytext); return R_IF; }
{thenStatement}         { yylval.str = strdup(yytext); return R_THEN; }
{elseStatement}         { yylval.str = strdup(yytext); return R_ELSE; }

{ID}                    { yylval.str = strdup(yytext); return ID; }

{operator}|{delimiter}  { return *yytext; }

{ignoredRegex}          { /* comments, spaces, newlines and tabs */ }

.                       { yyerror(yytext); }

%%

void yyerror(char* msg) {
  printf("Syntax error: %s -- ", msg);
  printf("At line: %d\n", yylineno);
}
