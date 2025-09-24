#include "../headers/simbolos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// using extern variable defined in main.c
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
    if (aux->info && aux->info->nombre == nombre) {
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
