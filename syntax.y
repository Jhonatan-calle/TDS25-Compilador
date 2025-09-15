%code requires {
    #include "headers/main.h"
    #include "headers/utils.h"
}

%{
    #include <stdlib.h>
    #include <stdio.h>
    #include <stdarg.h>
    #include <string.h>

    void yyerror(char* msg);
%}


%type program declaration_list
%type type param_list block declaration
%type statement_list statement method_call expr literal
%token T_EOS
%token V_FALSE V_NUM V_TRUE
%token R_VOID R_INTEGER R_BOOL
%token R_EXTERN R_RETURN R_PROGRAM
%token ID
%token R_IF R_THEN R_ELSE R_WHILE
%token OP_AND OP_OR OP_EQ

%left OP_AND
%left OP_OR
%left OP_EQ
%left '<' '>'
%left '+' '-'
%left '*' '/' '%'
%precedence '!' UMINUS  // Precedence for negation and unary substract


%%

program
    : R_PROGRAM '{' declaration_list '}'  { print_if_debug("todo good\n"); }
    ;

declaration_list
    : %empty /* Lambda */
    | declaration_list declaration
    ;

declaration
    : type ID decl_rest
    | R_VOID ID decl_rest
    ;

decl_rest
    : '=' expr T_EOS           // variable declaration
    | '(' param_list ')' block // method declaration
    | '(' param_list ')' R_EXTERN T_EOS // extern method
    ;

param_list
    : %empty /* Lambda */
    | param_list_nonempty
    ;

param_list_nonempty
    : type ID
    | param_list_nonempty ',' type ID
    ;

block
    : '{' declaration_list statement_list '}'
    ;

type
    : R_INTEGER
    | R_BOOL
    ;

statement_list
    : %empty /* Lambda */
    | statement_list statement
    ;

statement
    : ID '=' expr T_EOS
    | method_call T_EOS
    | R_IF '(' expr ')' R_THEN block
    | R_IF '(' expr ')' R_THEN block R_ELSE block
    | R_WHILE expr block
    | R_RETURN expr T_EOS
    | R_RETURN T_EOS
    | T_EOS
    | block
    ;

method_call
    : ID '(' arg_list ')'
    ;

arg_list
    : %empty /* Lambda */
    | arg_list_nonempty
    ;

arg_list_nonempty
    : expr
    | arg_list_nonempty ',' expr
    ;

expr
    : ID
    | method_call
    | literal
    | '-' expr %prec UMINUS
    | '!' expr
    | expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | expr '%' expr
    | expr '<' expr
    | expr '>' expr
    | expr OP_EQ expr
    | expr OP_AND expr
    | expr OP_OR expr
    | '(' expr ')'
    ;

literal
    : V_NUM
    | V_TRUE
    | V_FALSE
    ;

%%

int main(int argc, char *argv[])
{
	return compiler_main(argc, argv);
}
