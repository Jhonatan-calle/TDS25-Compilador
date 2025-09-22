#ifndef AST_H
#define AST_H

#include "types.h"
#include "symbols.h"

extern TablaSimbolos *t;

typedef struct AST
{
  TipoNodo type;
  Simbolo *info;
  int child_count;
  struct AST **childs;
} AST;

// API del AST
AST *new_node(TipoNodo type, int child_count, ...);
AST *append_child(AST *list, AST *child);
void free_ast(AST *node);

void print_ast(AST *node, int level);
void print_indent(int level);
const char *tipoNodoToStr(TipoNodo type);
const char *tipoDatoToStr(Tipos type);

#endif // AST_H
