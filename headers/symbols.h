#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "forward_declarations.h"
#include "types.h"

struct Symbol {
  int offset;
  MethodCategory category;
  char *name;  // name identifier
  Types t_var; // type
  int value;   // value --- Variables only
  // --- Methods only
  int num_params;
  Types *parameter_types;
  AST *body;
};

struct ScopeNode {
  Symbol *info;
  ScopeNode *next;
  ScopeNode *prev;
};

// Using fictional as a border between scopes
struct Scope {
  ScopeNode *head;
  ScopeNode *tail;
};

extern Scope *scope;

void initialize_scope();
void insert_symbol(Symbol *e);
Symbol *search_symbol_globally(char *name);
Symbol *search_symbol_locally(char *name);
void free_scope();
char *symbol_to_string(Symbol *s);

#endif // SYMBOLS_H
