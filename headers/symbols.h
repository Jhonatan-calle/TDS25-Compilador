#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdlib.h>

#include "ast.h"
#include "forward_declarations.h"
#include "types.h"

struct Simbolo {
  MethoCategory categoria;
  char *nombre; // identificador
  Tipos tVar;   // tipo
  int valor;    // valor ---SOLO VARIABLES
  //-----SOLO METODOS
  int num_params;
  Tipos *param_tipos;
  AST *cuerpo;
};

struct ScopeNode {
  Simbolo *info;
  ScopeNode *next;
  ScopeNode *prev;
};

// voy a usar ficiticio como frontera entre scopes
struct Scope {
  ScopeNode *head;
  ScopeNode *tail;
};

extern Scope *scope;

void inicialize_scope();
void insertar_simbolo(Simbolo *e);
Simbolo *buscar_simbolo(char *nombre);
Simbolo *crear_simbolo_variable(AST *node, AST *exp, Tipos tipoIdentificador,
                                char *nombre);
void liberar_tabla();
void print_symtable();
Simbolo *buscar_simbolo_local(char *nombre);
void liberar_scope();

#endif // SYMBOLS_H
