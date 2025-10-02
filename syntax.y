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


%type <node> program declaration_list declaration expr literal param  param_list_nonempty
%type <node> statement_list arg_list statement param_list arg_list_nonempty cuerpo else_cuerpo
%token <id> ID
%type <tipo> type
%token T_EOS
%token V_FALSE V_TRUE
%token <val> V_NUM
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
%precedence NOT
%precedence UMINUS

%%

program
  : R_PROGRAM '{' declaration_list '}' {
    $$ = new_node(TR_PROGRAM, 1, $3);
    root = $$;
    gen_assembly_if_assembly_flag(root);
    gen_inter_code_if_inter_code_flag(root);
    print_if_debug_flag("End of compilation.\n");
  }
  ;

declaration_list
  : %empty {
    $$ = new_node(TR_DECLARATION_LIST, 0);
  }
  | declaration_list declaration {
    $$ = append_child($1, $2);
  }
  ;

declaration
  : type ID '=' expr T_EOS {
    $$ = new_node(TR_VAR_DECLARATION, 1, $1, $2, $4);
  }
  | type ID '(' {
    inicialize_scope();
  } param_list ')' cuerpo {
    liberar_scope();
    $$ = new_node(TR_METHOD_DECLARATION, 4, $1, $2, $5, $7);
  }
  ;

cuerpo
  :'{' declaration_list statement_list '}' {
    $$ = new_node(TR_BLOCK, 2, $2, $3);
  }
  | R_EXTERN T_EOS {
    $$ = new_node(TR_EXTERN, 0);
  }
  ;

param_list
  : %empty {
    $$ = NULL;
  }
  | param_list_nonempty {
    $$ = $1;
  }
  ;

param_list_nonempty
  : param {
    $$ = new_node(TR_PARAM_LIST, 1, $1);
  }
  | param_list_nonempty ',' param {
    $$ = append_child($1, $3);
  }
  ;

param
  : type ID {
    $$ = new_node(TR_PARAM, 2, $1, $2);
  }
  ;

type
  : R_INTEGER {
    $$ = T_INT;
  }
  | R_BOOL {
    $$ = T_BOOL;
  }
  | R_VOID {
    $$ = T_VOID;
  }
  ;

statement_list
  : %empty {
    $$ = new_node(TR_SENTENCES_LIST, 0);
  }
  | statement_list statement {
    $$ = append_child($1, $2);
  }
  ;

statement
  : ID '=' expr T_EOS {
    $$ = new_node(TR_ASSIGN, 2, $1, $3);
  }
  | ID '(' {
    inicialize_scope();
  } arg_list ')' T_EOS {
    $$ = new_node(TR_INVOCATION, 2, $1, $4);
    liberar_scope();
  }
  | R_IF '(' expr ')' R_THEN '{' {
    inicialize_scope();
  } declaration_list statement_list '}' else_cuerpo {
    $$ = new_node(TR_IF_STATEMENT, 4, $3, $8, $9, $11);
    liberar_scope();
  }
  | R_WHILE '(' expr ')' '{' {
    inicialize_scope();
  } declaration_list statement_list '}' {
    $$ = new_node(TR_WHILE_STATEMENT, 3, $3, $7, $8);
    liberar_scope();
  }
  | R_RETURN expr T_EOS {
    $$ = new_node(TR_RETURN, 1, $2);
  }
  | R_RETURN T_EOS {
    $$ = new_node(TR_RETURN, 0);
  }
  | T_EOS {
    $$ = NULL;
  }
  | '{' {
    inicialize_scope();
  } declaration_list statement_list '}' {
    $$ = new_node(TR_BLOCK, 2, $3, $4);
    liberar_scope();
  }
  ;

else_cuerpo
  : %empty {
    $$ = NULL;
  }
  | R_ELSE '{' {
    inicialize_scope();
  } declaration_list statement_list '}' {
    $$ = new_node(TR_ELSE_BODY, 2, $4, $5);
    liberar_scope();
  }

arg_list
  : %empty {
    $$ = NULL;
  }
  | arg_list_nonempty {
    $$ = $1;
  }
  ;

arg_list_nonempty
  :  expr {
    $$ = new_node(TR_ARG_LIST, 1, $1);
  }
  | arg_list',' expr {
    $$ = append_child($1, $3);
  }
  ;

expr
  : ID {
    $$ = new_node(TR_IDENTIFIER, 1, $1);
  }
  | ID '(' arg_list ')' {
    inicialize_scope();
    $$ = new_node(TR_INVOCATION, 2, $1, $3);
    liberar_scope();
  }
  | literal {
    $$ = $1;
  }
  | '!' expr %prec NOT {
    $$ = new_node(TR_LOGIC_NEGATION, 1, $2);
  }
  | '-' expr %prec UMINUS {
    $$ = new_node(TR_ARITHMETIC_NEGATION, 1, $2);
  }
  | expr '+' expr {
    $$ = new_node(TR_ADDITION, 2, $1, $3);
  }
  | expr '-' expr {
    $$ = new_node(TR_SUBSTRACTION, 2, $1, $3);
  }
  | expr '*' expr {
    $$ = new_node(TR_MULTIPLICATION, 2, $1, $3);
  }
  | expr '/' expr {
    $$ = new_node(TR_DIVITION, 2, $1, $3);
  }
  | expr '%' expr {
    $$ = new_node(TR_MODULO, 2, $1, $3);
  }
  | expr '<' expr {
    $$ = new_node(TR_LESS_THAN, 2, $1, $3);
  }
  | expr '>' expr {
    $$ = new_node(TR_GREATER_THAN, 2, $1, $3);
  }
  | expr OP_EQ expr {
    $$ = new_node(TR_LOGIC_EQUAL, 2, $1, $3);
  }
  | expr OP_AND expr {
    $$ = new_node(TR_AND, 2, $1, $3);
  }
  | expr OP_OR expr {
    $$ = new_node(TR_OR, 2, $1, $3);
  }
  | '(' expr ')' {
    $$ = $2;
  }
  ;

literal
  : V_NUM {
    $$ = new_node(TR_VALUE, 0, T_INT, $1);
  }
  | V_TRUE {
    $$ = new_node(TR_VALUE, 0, T_BOOL, 0);
  }
  | V_FALSE {
    $$ = new_node(TR_VALUE, 0, T_BOOL, 1);
  }
  ;

%%

int main(int argc, char *argv[])
{
	return compiler_main(argc, argv);
}

