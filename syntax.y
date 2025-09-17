%code requires {
    #include "headers/tipos.h"
    #include "headers/simbolos.h"
    #include "headers/ast.h"
    #include "headers/utils.h"

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


%type <node> program declaration_list declaration expr literal block
%type <tipo> type 
%type param_list 
%type statement_list statement method_call 
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
%preceence for negation and unary substract


%%

program
    : R_PROGRAM '{' declaration_list '}'  
    { 
      $$ = new_node(TR_PROGRAMA,1,$3);
      root = $$;
      print_ast($$,0);
      free_ast($$);
    }
    ;

declaration_list
    : %empty
        { $$ = NULL; }  /* lista vacía */
    | declaration
        { $$ = new_node(TR_DECLARATION_LIST, 1, $1); }  /* un nodo */
    | declaration_list declaration
        { $$ = append_child($1, $2); }  /* agregar al árbol existente */
    ;

declaration
    : type ID decl_rest
        { $$ = new_node(TR_DECLARACION,3, $1,$2,$3); }  /* un nodo */ 
    ;

decl_rest
    : '=' expr T_EOS           // variable declaration
        { $$ = new_node(TR_EXPRESION,1, $1); }  /* un nodo */ 
    | '(' param_list ')' block // method declaration
        { $$ = new_node(TR_PERFIL,2, $2, $4); }  /* un nodo */ 
    | '(' param_list ')' R_EXTERN T_EOS // extern method
        { $$ = new_node(TR_PERFIL_EXTERN, $1); }  /* un nodo */ 
    ;

param_list
    : %empty /* Lambda */
        { $$ = NULL; }  /* lista vacía */
    | param_list_nonempty
        { $$ = $1; }  /* lista vacía */
    ;

param_list_nonempty
    : type ID
    { $$ = new_node(TR_DECLARATION_LIST, 1, $1); }  /* un nodo */
    | param_list_nonempty ',' type ID
    { $$ = new_node(TR_DECLARATION_LIST, 1, $1); }  /* un nodo */
    ;

block
    : '{' declaration_list statement_list '}'
        { $$ = new_node(TR_BLOCK, 2, $1, $2);}

type
    : R_INTEGER { $$ = T_INT}
    | R_BOOL { $$ = T_BOOL }
    | R_VOID { $$ = T_VOID }
    ;

statement_list
    : %empty /* Lambda */
    { $$ = NULL}
    | statement
        { $$ = new_node(TR_LISTA_SENTENCIAS, 1, $1); }  /* un nodo */
    | statement_list statement
        { $$ = append_child($1, $2); }  /* agregar al árbol existente */
    ;

statement
    : ID '=' expr T_EOS
      {$$ = new_node(TR_ASIGNACION,2,$1,$3);}
    | method_call T_EOS
      {$$ = new_node(TR_INVOCATION,1,$1);}
    | R_IF '(' expr ')' R_THEN block
      {$$ = new_node(TR_IF_STATEMENT,2,$1,$2);}
    | R_IF '(' expr ')' R_THEN block R_ELSE block
      {$$ = new_node(TR_IF_ELSE_STATEMENT,2,$1,$2,$3);}
    | R_WHILE expr block
      {$$ = new_node(TR_WHILE_STATEMENT,1,$1,$2);}
    | R_RETURN expr T_EOS
      {$$ = new_node(TR_RETURN,1,$1);}
    | R_RETURN T_EOS
      {$$ = new_node(TR_RETURN,0);}
    | T_EOS
       { $$ = NULL}
    | block
       { $$ = $1}
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
