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

%type <node> program declaration_list declaration expr literal param
%type <node> block statement_list arg_list decl_rest statement param_list
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
  : R_PROGRAM '{' declaration_list '}'
  {
    print_if_debug_flag("[DEBUG PARSER] creando nodo tipo=TR_PROGRAM");
    $$ = new_node(TR_PROGRAM, 1, $3);
    root = $$;
    gen_assembly_if_assembly_flag(root);
    free_ast($$);
  }
  ;

declaration_list
  : %empty
  {
    $$ = new_node(TR_VAR_DECLARATION_LIST, 0);
    print_if_debug_flag("[DEBUG PARSER] creando nodo tipo=TR_VAR_DECLARATION_LIST");
  }
  | declaration_list declaration
  {
    $$ = append_child($1, $2);
  }
  ;

declaration
  : type ID decl_rest
  {
    print_if_debug_flag("[DEBUG PARSER] creando nodo tipo=TR_DECLARATION");
    $$ = new_node(TR_DECLARATION, 3, $1, $2, $3);
  }
  ;

decl_rest
  : '=' expr T_EOS           // variable declaration
      { $$ = new_node(TR_VAR_DECLARATION, 1, $2); }  /* un nodo */
  | '(' param_list ')' block // method declaration { $$ = new_node(TR_METHOD,2, $2, $4); }  /* un nodo */
      { $$ = new_node(TR_METHOD, 2, $2, $4); }  /* un nodo */
  | '(' param_list ')' R_EXTERN T_EOS // extern method
      { $$ = new_node(TR_METHOD_EXTERN, 1, $2); }  /* un nodo */
  ;

param_list
  : %empty /* Lambda */
      { $$ = new_node(TR_PARAM_LIST, 0);}
  | param_list ',' param
      { $$ = append_child($1, $3);}
  ;

param
  : type ID
    { $$ = new_node(TR_PARAM, 2, $1, $2); }  /* un nodo */
  ;

block
  : '{' declaration_list statement_list '}'
      { $$ = new_node(TR_BLOCK, 2, $2, $3);}
  ;

type
  : R_INTEGER { $$ = T_INT;}
  | R_BOOL { $$ = T_BOOL ;}
  | R_VOID { $$ = T_VOID ;}
  ;

statement_list
  : %empty /* Lambda */
      { $$ = new_node(TR_SENTENCES_LIST, 0); }  /* un nodo */
  | statement_list statement
      { $$ = append_child($1, $2); }  /* agregar al árbol existente */
  ;

statement
  : ID '=' expr T_EOS
    {$$ = new_node(TR_ASSIGN,2,$1,$3);}
  | ID '(' arg_list ')'
      {$$ = new_node(TR_INVOCATION,2,$1,$3);}
  | R_IF '(' expr ')' R_THEN block
    {$$ = new_node(TR_IF_STATEMENT,2,$3,$6);}
  | R_IF '(' expr ')' R_THEN block R_ELSE block
    {$$ = new_node(TR_IF_ELSE_STATEMENT,3,$3,$6,$8);}
  | R_WHILE expr block
    {$$ = new_node(TR_WHILE_STATEMENT,2,$2,$3);}
  | R_RETURN expr T_EOS
    {$$ = new_node(TR_RETURN,1,$2);}
  | R_RETURN T_EOS
    {$$ = new_node(TR_RETURN,0);}
  | T_EOS
      { $$ = NULL;}
  | block
      { $$ = $1;}
  ;


arg_list
  : %empty /* Lambda */
      { $$ = new_node(TR_ARG_LIST, 0); }
  | arg_list',' expr
      { $$ = append_child($1, $3); }  /* agregar al árbol existente */
  ;

expr
  : ID
      { $$ = new_node(TR_IDENTIFIER, 1, $1); }
  | ID '(' arg_list ')'
      {$$ = new_node(TR_INVOCATION,1,$1,$3);}
  | literal
      { $$ = $1;}
  | '!' expr %prec NOT
      {$$ = new_node(TR_LOGIC_NEGATION,1,$2);}
  | '-' expr %prec UMINUS
      {$$ = new_node(TR_ARITHMETIC_NEGATION,1,$2);}
  | expr '+' expr
      { $$ = new_node(TR_ADDITION, 2, $1, $3); }
  | expr '-' expr
      { $$ = new_node(TR_SUBSTRACTION, 2, $1, $3); }
  | expr '*' expr
      { $$ = new_node(TR_MULTIPLICATION, 2, $1, $3); }
  | expr '/' expr
      { $$ = new_node(TR_DIVITION, 2, $1, $3); }
  | expr '%' expr
      { $$ = new_node(TR_MODULO, 2, $1, $3); }
  | expr '<' expr             //---------------que pasa con menor igual o mayor igual
      { $$ = new_node(TR_LESS_THAN, 2, $1, $3); }
  | expr '>' expr
      { $$ = new_node(TR_GREATER_THAN, 2, $1, $3); }
  | expr OP_EQ expr
      { $$ = new_node(TR_LOGIC_EQUAL, 2, $1, $3); }
  | expr OP_AND expr
      { $$ = new_node(TR_AND, 2, $1, $3); }
  | expr OP_OR expr
      { $$ = new_node(TR_OR, 2, $1, $3); }
  | '(' expr ')'
      { $$ = $2; }
  ;

literal
  : V_NUM
    {$$ = new_node(TR_VALUE,0,T_INT, $1);}
  | V_TRUE
    {$$ = new_node(TR_VALUE,0,T_BOOL, 0);}
  | V_FALSE
    {$$ = new_node(TR_VALUE,0,T_BOOL, 1);}
  ;

%%

int main(int argc, char *argv[])
{
	return compiler_main(argc, argv);
}
