#include "../headers/symbols.h"

// Using extern variable defined in main.c
extern Scope *scope;

void initialize_scope() {
  // border mark
  ScopeNode *border = malloc(sizeof(ScopeNode));
  border->info = NULL;
  border->prev = scope->tail;
  if (scope->tail) {
    scope->tail->next = border;
  }
  scope->tail = border;
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
Symbol *search_symbol_globally(char *name) {
  ScopeNode *aux = scope->tail;
  while (aux) {
    if (aux->info && strcmp(aux->info->name, name) == 0) {
      return aux->info;
    }
    aux = aux->prev;
  }
  return NULL; // not found
}

// Searchs until the first boundary
Symbol *search_symbol_locally(char *name) {
  ScopeNode *aux = scope->tail;
  while (aux->info) {
    if (strcmp(aux->info->name, name) == 0) {
      return aux->info;
    }
    aux = aux->prev;
  }
  return NULL; // not found
}

void free_scope() {
  ScopeNode *aux = scope->tail;
  while (aux->info) {
    scope->tail = aux->prev;
    free(aux);
    aux = scope->tail;
  }
  scope->tail = aux->prev;
  free(aux); // free the border mark
}

char *symbol_to_string(Symbol *s) {
  if (!s) {
    return strdup("(symbol=NULL)");
  }

  const char *cat_str = "UNKNOWN";
  // Heuristic/default names for category if enum values are available
  // Assume S_VAR / S_FUNC exist in MethodCategory (as in previous branches)
  if (s->category == S_VAR)
    cat_str = "VAR";
  if (s->category == S_FUNC)
    cat_str = "FUNC";

  const char *name = s->name ? s->name : "<anon>";
  const char *type_str = data_types_to_string(s->t_var);

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

  if (s->category == S_FUNC) {
    // Function symbol description
    int w = snprintf(buf + used, cap - used, ", num_params=%d, param_types=[",
                     nparams);
    if (w > 0)
      used += (size_t)w;

    for (int i = 0; i < nparams; i++) {
      const char *pt = data_types_to_string(
          s->parameter_types ? s->parameter_types[i] : s->t_var);
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
    w = snprintf(buf + used, cap - used, "], body=%p)", (void *)s->body);
    if (w > 0)
      used += (size_t)w;
  } else {
    // Variable symbol description
    int w = snprintf(buf + used, cap - used, ", value=%d)", s->value);
    if (w > 0)
      used += (size_t)w;
  }

  return buf;
}
