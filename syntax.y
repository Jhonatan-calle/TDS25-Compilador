%code requires {
    #include "headers/ast.h"
    #include "headers/main.h"
    #include "headers/utils.h"
    #include "headers/symbols.h"
    #include "headers/types.h"
}
%{
    #include <stdlib.h>
    #include <stdio.h>
    #include <stdarg.h>
    #include <string.h>

    void yyerror(char* msg);
%}

%union {
    struct AST *node;
    Tipos tipo;
    char *id;
    int val;
}

%code {
    AST *root;
}

%type <node> program var_decl_block var_decl
%type <node> method_decl_block method_decl method_end
%type <node> param_list param_list_nonempty
%type <node> block opened_block
%type <node> statement_list statement expr literal
%type <node> arg_list
%type <tipo> type

%token <tipo> ID V_FALSE V_NUM V_TRUE
%token T_EOS
%token R_VOID R_INTEGER R_BOOL
%token R_EXTERN R_RETURN R_PROGRAM
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
    : R_PROGRAM '{' var_decl_block method_decl_block '}'
    {
      $$ = new_node(TR_PROGRAMA, 2, $3, $4);
      root = $$;
      print_ast($$, 0);
      free_ast($$);
    }
    | R_PROGRAM '{' method_decl_block '}'
    {
      $$ = new_node(TR_PROGRAMA, 1, $3);
      root = $$;
      print_ast($$,0);
      free_ast($$);
    }
    ;

var_decl_block
    : var_decl_block var_decl
    { $$ = append_child($1, $2); }
    | var_decl
    { $$ = new_node(TR_DECLARATION_LIST, 1, $1); }
    ;

var_decl
    : type ID '=' expr T_EOS
    { $$ = new_node(TR_EXPRESION,1, $1); }
    ;

method_decl_block
    : method_decl method_decl_block
    { $$ = append_child($2, $1); }
    | method_decl
    { $$ = new_node(TR_DECLARATION_LIST, 1, $1); }
    ;

method_decl
    : type ID '(' { printf("check of ID"); } method_end { printf("do smth"); }
    | R_VOID ID '(' { printf("do smth"); } method_end { printf("do smth"); }
    ;

method_end
    : param_list ')' block
    | param_list ')' R_EXTERN T_EOS
    ;

param_list
    : %empty /* Lambda */
    { $$ = NULL; }
    | param_list_nonempty
    { $$ = $1; }
    ;

param_list_nonempty
    : type ID
    { $$ = new_node(TR_DECLARATION_LIST, 1, $1); }
    | type ID ',' param_list_nonempty
    { $$ = new_node(TR_DECLARATION_LIST, 1, $4); }
    ;

block
    : '{' opened_block
    { $$ = new_node(TR_BLOCK, 1, $2); }
    ;

opened_block
    : var_decl_block statement_list '}'
    { $$ = new_node(TR_BLOCK, 2, $1, $2); }
    | statement_list '}'
    { $$ = new_node(TR_BLOCK, 1, $1); }
    ;

statement_list
    : %empty /* Lambda */
    { $$ = NULL; }
    | statement_list statement
    { $$ = append_child($1, $2); }
    ;

statement
    : ID '=' expr T_EOS
    { $$ = new_node(TR_ASIGNACION, 2, $1 ,$3); }
    | ID '(' arg_list ')' T_EOS
    { $$ = new_node(TR_INVOCATION, 1, $3); }
    | R_IF '(' expr ')' R_THEN block
    { $$ = new_node(TR_IF_STATEMENT, 2, $3, $6); }
    | R_IF '(' expr ')' R_THEN block R_ELSE block
    { $$ = new_node(TR_IF_ELSE_STATEMENT, 3, $3, $6, $8); }
    | R_WHILE expr block
    { $$ = new_node(TR_WHILE_STATEMENT, 2, $2, $3); }
    | R_RETURN expr T_EOS
    { $$ = new_node(TR_RETURN, 1, $2); }
    | R_RETURN T_EOS
    { $$ = new_node(TR_RETURN, 0); }
    | T_EOS
    { $$ = NULL; }
    | block
    { $$ = $1; }
    ;

arg_list
    : %empty /* Lambda */
    { $$ = NULL; }
    | expr ',' arg_list
    { $$ = append_child($3, $1); }
    | expr
    { $$ = new_node(TR_ARG_LIST, 1, $1); }
    ;

expr
    : ID
    { $$ = new_node(TR_IDENTIFICADOR, 1, $1); }
    | ID '(' arg_list ')'
    { $$ = new_node(TR_INVOCATION, 1, $3); }
    | literal
    { $$ = $1 ;}
    | '-' expr %prec UMINUS
    { $$ = new_node(TR_NEGATIVE_NUMBER, 1, $2); }
    | '!' expr
    { $$ = new_node(TR_NEGATION, 1, $2); }
    | expr '+' expr
    { $$ = new_node(TR_SUMA, 2, $1, $3); }
    | expr '-' expr
    { $$ = new_node(TR_SUBSTRACTION, 2, $1, $3); }
    | expr '*' expr
    { $$ = new_node(TR_MULTIPLICACION, 2, $1, $3); }
    | expr '/' expr
    { $$ = new_node(TR_DIVISION, 2, $1, $3); }
    | expr '%' expr
    { $$ = new_node(TR_MODULO, 2, $1, $3); }
    | expr '<' expr
    { $$ = new_node(TR_LESS_THAN, 2, $1, $3); }
    | expr '>' expr
    { $$ = new_node(TR_GREATER_THAN, 2, $1, $3); }
    | expr OP_EQ expr
    { $$ = new_node(TR_EQ, 2, $1, $3); }
    | expr OP_AND expr
    { $$ = new_node(TR_AND, 2, $1, $3); }
    | expr OP_OR expr
    { $$ = new_node(TR_OR, 2, $1, $3); }
    | '(' expr ')'
    { $$ = $2; }
    ;

type
    : R_INTEGER
    { $$ = R_INTEGER; }
    | R_BOOL
    { $$ = R_BOOL; }
    ;

literal
    : V_NUM
    { $$ = new_node(TR_VALOR, 0, T_INT, $1); }
    | V_TRUE
    { $$ = new_node(TR_VALOR, 0, T_BOOL, 1); }
    | V_FALSE
    { $$ = new_node(TR_VALOR, 0, T_BOOL, 0); }
    ;

%%

int main(int argc, char *argv[])
{
	return compiler_main(argc, argv);
}
