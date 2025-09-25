#ifndef SIMBOLOS_H
#define SIMBOLOS_H

#include "forward_declarations.h"
#include "tipos.h"
#include <stdlib.h>

struct Simbolo {
  Categoria categoria;
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
Simbolo *buscar_simbolo_local(char *nombre);
void liberar_scope();

#endif // SIMBOLOS_H
