#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdlib.h>

#include "ast.h"
#include "forward_declarations.h"
#include "types.h"

struct Symbol {
  int offset;
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
extern int DYNAMIC_OFFSET;

void enter_scope_offset();
void exit_scope_offset();
int get_offset();
void initialize_scope();
void insert_symbol(Symbol *e);
Symbol *search_symbol_globally(char *nombre);
Symbol *search_symbol_locally(char *nombre);
void free_scope();
char *symbol_to_string(Symbol *s);

#endif // SYMBOLS_H
