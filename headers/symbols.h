#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdlib.h>

#include "ast.h"
#include "forward_declarations.h"
#include "types.h"

struct Symbol {
  MethodCategory categoria;
  char *nombre; // identificador
  Tipos tVar;   // tipo
  int valor;    // valor ---SOLO VARIABLES
  //-----SOLO METODOS
  int num_params;
  Tipos *param_tipos;
  AST *cuerpo;
};

struct ScopeNode {
  Symbol *info;
  ScopeNode *next;
  ScopeNode *prev;
};

// voy a usar ficiticio como frontera entre scopes
struct Scope {
  ScopeNode *head;
  ScopeNode *tail;
};

extern Scope *scope;

void initialize_scope();
void insert_symbol(Symbol *e);
Symbol *search_symbol_globally(char *nombre);
Symbol *search_symbol_locally(char *nombre);
void free_scope();

#endif // SYMBOLS_H
