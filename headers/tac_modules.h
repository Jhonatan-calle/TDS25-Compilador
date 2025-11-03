#include "ast.h"
#include "forward_declarations.h"
#include "symbols.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *get_if_name_labels(OpCode type);
void tac_var_dec_module(AST *root, Symbol *exp);
void tac_method_dec_module(AST *root);
void tac_param_module(AST *root);
void tac_block_module(AST *root);
void tac_assign_module(AST *root, Symbol *exp);
void tac_invocation_module(AST *root);
void tac_if_statement_module(AST *root, Symbol *L_end, Symbol *exp);
void tac_else_body_module(AST *root);
void tac_while_statement_module(AST *root, Symbol *L_end, Symbol *exp);
void tac_return_module(AST *root);
void tac_args_list_module(AST *root);
