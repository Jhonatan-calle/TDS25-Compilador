#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/symbols.h"

// Using extern variable defined in main.c
extern Scope *scope;

void initialize_scope() {
  ScopeNode *frontera = malloc(sizeof(ScopeNode));
  frontera->info = NULL; // marca de frontera
  frontera->prev = scope->tail;
  if (scope->tail) {
    scope->tail->next = frontera;
  }
  scope->tail = frontera;
}

void insert_symbol(Symbol *e) {
  ScopeNode *aux = scope->tail;

  aux = malloc(sizeof(ScopeNode));
  aux->info = e;
  aux->prev = scope->tail;
  aux->next = NULL;
  scope->tail->next = aux;
  scope->tail = aux;
}

// Searchs in all the scope
Symbol *search_symbol_globally(char *nombre) {
  ScopeNode *aux = scope->tail;
  while (aux) {
    if (aux->info && strcmp(aux->info->nombre, nombre) == 0) {
      return aux->info;
    }
    aux = aux->prev;
  }
  return NULL; // no encontrado
}

// Searchs until the first boundary
Symbol *search_symbol_locally(char *nombre) {
  ScopeNode *aux = scope->tail;
  while (aux->info) {
    if (strcmp(aux->info->nombre, nombre) == 0) {
      return aux->info;
    }
    aux = aux->prev;
  }
  return NULL; // no encontrado
}

void free_scope() {
  ScopeNode *aux = scope->tail;
  while (aux->info) {
    scope->tail = aux->prev;
    free(aux);
    aux = scope->tail;
  }
  scope->tail = aux->prev;
  free(aux); // elimino la frontera
}
