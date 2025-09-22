#ifndef AST_H
#define AST_H

#include "tipos.h"
#include "simbolos.h"

extern TablaSimbolos *global_scope_table;
extern TablaSimbolos *local_scope_table;

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

const char *tipoDatoToStr(Tipos type);
#endif // AST_H
