%{
    #include <stdlib.h>
 
%}


%type program var_decl_list method_decl_list 
%type var_decl method_decl type id param_list block 
%type statement_list statement method_call expr bin_op literal
%token T_EOS R_VOID R_EXTERN R_INTEGER R_BOOL ID R_ELSE R_IF 
%token R_THEN R_WHILE R_RETURN V_FALSE V_NUM V_TRUE

%%

program
    : '{' var_decl_list method_decl_list '}'  {printf("todo good")}
    ;

var_decl_list
    : /* empty */
    | var_decl_list var_decl
    ;

method_decl_list
    : /* empty */
    | method_decl_list method_decl
    ;

var_decl
    : type id '=' expr T_EOS
    ;

method_decl
    : type id '(' param_list ')' block
    | R_VOID id '(' param_list ')' block
    | type id '(' param_list ')' R_EXTERN ';'
    | R_VOID id '(' param_list ')' R_EXTERN ';'
    ;

param_list
    : /* empty */
    | param_list_nonempty
    ;

param_list_nonempty
    : type id
    | param_list_nonempty ',' type id
    ;

block
    : '{' var_decl_list statement_list '}'
    ;

type
    : R_INTEGER
    | R_BOOL
    ;

statement_list
    : /* empty */
    | statement_list statement
    ;

statement
    : id '=' expr ';'
    | method_call ';'
    | R_IF '(' expr ')' R_THEN block
    | R_IF '(' expr ')' R_THEN block R_ELSE block
    | R_WHILE expr block
    | R_RETURN expr ';'
    | ';'
    | block
    ;

method_call
    : id '(' arg_list ')'
    ;

arg_list
    : /* empty */
    | arg_list_nonempty
    ;

arg_list_nonempty
    : expr
    | arg_list_nonempty ',' expr
    ;

expr
    : id
    | method_call
    | literal
    | expr bin_op expr
    | '-' expr
    | '!' expr
    | '(' expr ')'
    ;

bin_op
    : '+' | '-' | '*' | '/' | '%'
    | '<' | '>' | '=''='
    | '&''&' | '|''|'
    ;

literal
    : V_NUM
    | V_TRUE
    | V_FALSE
    ;

id
    : ID
    ;

%%

