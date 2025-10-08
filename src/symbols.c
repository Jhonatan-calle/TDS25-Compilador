#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/symbols.h"

// Using extern variable defined in main.c
extern Scope *scope;

void inicialize_scope() {
  ScopeNode *frontera = malloc(sizeof(ScopeNode));
  frontera->info = NULL; // marca de frontera
  frontera->prev = scope->tail;
  if (scope->tail) {
    scope->tail->next = frontera;
  }
  scope->tail = frontera;
}

void insertar_simbolo(Simbolo *e) {
  ScopeNode *aux = scope->tail;

  // buscar en el scope actual
  while (aux->info) {
    if (aux->info->nombre == e->nombre) {
      // error el simbolo ya exite en el scope actual
    }
    aux = aux->prev;
  }

  aux = malloc(sizeof(ScopeNode));
  aux->info = e;
  aux->prev = scope->tail;
  aux->next = NULL;
  scope->tail->next = aux;
  scope->tail = aux;
}

Simbolo *buscar_simbolo(char *nombre) {
  ScopeNode *aux = scope->tail;
  while (aux) {
    if (aux->info && strcmp(aux->info->nombre, nombre) == 0) {
      return aux->info;
    }
    aux = aux->prev;
  }
  return NULL; // no encontrado
}

Simbolo *buscar_simbolo_local(char *nombre) {
  ScopeNode *aux = scope->tail;
  while (aux->info) {
    if (strcmp(aux->info->nombre, nombre) == 0) {
      return aux->info;
    }
    aux = aux->prev;
  }
  return NULL; // no encontrado
}

void liberar_scope() {
  ScopeNode *aux = scope->tail;
  while (aux->info) {
    scope->tail = aux->prev;
    free(aux);
    aux = scope->tail;
  }
  scope->tail = aux->prev;
  free(aux); // elimino la frontera
}

Simbolo *crear_simbolo_variable(AST *node, AST *exp, Tipos tipoIdentificador,
                                char *nombre) {
  Simbolo *id = malloc(sizeof(Simbolo));
  id->tVar = tipoIdentificador; // tipo (enum Tipos)
  id->nombre = nombre;          // identificador
  id->categoria = S_VAR;
  id->valor = exp->info->valor;
  insertar_simbolo(id);
  node->info = id;
  node->child_count = 0;
  return id;
}

Tipos get_expression_type(AST *node) {
  switch (node->type) {
  case TR_VALUE:
    return node->info->tVar;

  case TR_IDENTIFIER:
    return node->info ? node->info->tVar : T_VOID;

  case TR_ADDITION:
  case TR_SUBSTRACTION:
  case TR_MULTIPLICATION:
  case TR_DIVITION:
  case TR_MODULO:
    return T_INT;

  case TR_LOGIC_EQUAL:
  case TR_LESS_THAN:
  case TR_GREATER_THAN:
  case TR_AND:
  case TR_OR:
  case TR_LOGIC_NEGATION:
    return T_BOOL;

  default:
    return T_VOID;
  }
}
