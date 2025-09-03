%{
    #include <stdio.h>
    #include <stdlib.h>

    void yyerror(char* msg);
    int yylex(void);

    extern FILE *yyin;
%}

%type program var_decl_list method_decl_list
%type var_decl method_decl type ID param_list block
%type statement_list statement method_call expr literal
%token T_EOS
%token V_FALSE V_NUM V_TRUE
%token R_VOID R_INTEGER R_BOOL
%token R_EXTERN R_RETURN R_PROGRAM
%token ID
%token R_IF R_THEN R_ELSE R_WHILE
%token OP_AND OP_OR OP_EQ

%precedence '!' UMINUS  // Precedencia para negación y resta unaria
%left OP_AND
%left OP_OR
%left OP_EQ
%left '<' '>'
%left '+' '-'
%left '*' '/' '%'


%%

program
    : R_PROGRAM '{' method_decl_list var_decl_list  '}'  {printf("todo good");}
    ;

var_decl_list
    : %empty /* Lambda */
    | var_decl_list var_decl
    ;

method_decl_list
    : %empty /* Lambda */
    | method_decl_list method_decl
    ;

var_decl
    : type ID '=' expr T_EOS
    ;

method_decl
    : type ID '(' param_list ')' block
    | R_VOID ID '(' param_list ')' block
    | type ID '(' param_list ')' R_EXTERN T_EOS
    | R_VOID ID '(' param_list ')' R_EXTERN T_EOS
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
    : '{' var_decl_list statement_list '}'
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


int main(int argc,char *argv[]){
	++argv,--argc;
	if (argc > 0)
		yyin = fopen(argv[0],"r");
	else
		yyin = stdin;

	yyparse();
}
