#ifndef AST_H
#define AST_H

#include <stdlib.h>

#include "forward_declarations.h"
#include "symbols.h"
#include "types.h"
#include "utils.h"

struct AST {
  TipoNodo type;
  Simbolo *info;
  int child_count;
  struct AST **childs;
};

// API del AST
AST *new_node(TipoNodo type, int child_count, ...);
AST *append_child(AST *list, AST *child);
void free_ast(AST *node);

const char *tipoDatoToStr(Tipos type);
const char *tipoNodoToStr(TipoNodo t);
#endif // AST_H
