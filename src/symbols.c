#include <stdbool.h>
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

char *symbol_to_string(Symbol *s) {
  if (!s) {
    return strdup("(symbol=NULL)");
  }

  const char *cat_str = "UNKNOWN";
  // Heuristic/default names for category if enum values are available
  // Assume S_VAR / S_FUNC exist in MethodCategory (as in previous branches)
  if (s->categoria == S_VAR)
    cat_str = "VAR";
  if (s->categoria == S_FUNC)
    cat_str = "FUNC";

  const char *name = s->nombre ? s->nombre : "<anon>";
  const char *type_str = tipoDatoToStr(s->tVar);

  // Estimate capacity: base + per-parameter room
  int nparams = s->num_params;
  size_t cap = 256 + (nparams > 0 ? (size_t)nparams * 24 : 0);
  char *buf = malloc(cap);
  if (!buf) {
    return strdup("(symbol=OOM)");
  }

  int written = snprintf(buf, cap, "(offset=%d, category=%s, name=%s, type=%s",
                         s->offset, cat_str, name, type_str);
  if (written < 0) {
    free(buf);
    return strdup("(symbol=snprintf_error)");
  }

  size_t used = (size_t)written;

  if (s->categoria == S_FUNC) {
    // Function symbol description
    int w = snprintf(buf + used, cap - used, ", num_params=%d, param_types=[",
                     nparams);
    if (w > 0)
      used += (size_t)w;

    for (int i = 0; i < nparams; i++) {
      const char *pt =
          tipoDatoToStr(s->param_tipos ? s->param_tipos[i] : s->tVar);
      w = snprintf(buf + used, cap - used, "%s%s", (i == 0 ? "" : ","), pt);
      if (w > 0)
        used += (size_t)w;
      if (used + 16 >= cap) {
        // grow buffer conservatively if needed
        cap *= 2;
        char *nbuf = realloc(buf, cap);
        if (!nbuf) {
          // fall back to closing early
          break;
        }
        buf = nbuf;
      }
    }
    w = snprintf(buf + used, cap - used, "], body=%p)", (void *)s->cuerpo);
    if (w > 0)
      used += (size_t)w;
  } else {
    // Variable symbol description
    int w = snprintf(buf + used, cap - used, ", value=%d)", s->valor);
    if (w > 0)
      used += (size_t)w;
  }

  return buf;
}