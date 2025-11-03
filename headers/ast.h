#ifndef AST_H
#define AST_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast_modules.h"
#include "forward_declarations.h"
#include "symbols.h"
#include "types.h"
#include "utils.h"

struct AST {
  NodeType type;
  Symbol *info;
  int child_count;
  struct AST **children;
};

// API of the AST
AST *new_node(NodeType type, int child_count, ...);
AST *append_child(AST *list, AST *child);
void print_ast(AST *node, int depth);
void save_ast_in_file(AST *root);
void free_ast(AST *node);
void print_generated_ast_if_debug_flag();

const char *data_types_to_string(Types type);
const char *node_type_to_string(NodeType t);
#endif // AST_H
