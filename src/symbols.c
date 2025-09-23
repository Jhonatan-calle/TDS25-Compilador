#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/symbols.h"

// Using extern variable defined in main.c
extern TablaSimbolos *global_table;

TablaSimbolos *crear_tabla(size_t capacidad_inicial) {
  TablaSimbolos *t = malloc(sizeof(TablaSimbolos));
  t->capacidad = capacidad_inicial;
  t->usados = 0;
  t->tabla = malloc(capacidad_inicial * sizeof(Simbolo *));
  return t;
}

void insertar_simbolo(Simbolo *e) {
  if (global_table->usados >= global_table->capacidad) {
    global_table->capacidad *= 2;
    global_table->tabla = realloc(global_table->tabla,
                                  global_table->capacidad * sizeof(Simbolo *));
  }
  global_table->tabla[global_table->usados] = e;
  global_table->usados++;
}

Simbolo *buscar_simbolo(char *nombre) {
  for (size_t i = 0; i < global_table->usados; i++)
    if (strcmp(global_table->tabla[i]->nombre, nombre) == 0)
      return global_table->tabla[i];
  return NULL; // no encontrado
}

Simbolo *crear_simbolo_variable(AST *node, AST *exp, Tipos tipoIdentificador,
                                char *nombre, int valor) {
  Simbolo *id = malloc(sizeof(Simbolo));
  id->tVar = tipoIdentificador; // tipo (enum Tipos)
  id->nombre = nombre;          // identificador
  id->categoria = S_VAR;
  id->valor = valor;
  insertar_simbolo(id);
  node->info = id;
  node->child_count = 0;
  return id;
}

void crear_simbolo_metodo(AST *node, AST *params, AST *cuerpo,
                              Tipos tipoIdentificador, char *nombre) {
  Simbolo *simbol = malloc(sizeof(Simbolo));
  simbol->tVar = tipoIdentificador; // tipo (enum Tipos)
  simbol->nombre = nombre;          // identificador
  simbol->categoria = S_FUNC;
  simbol->num_params = params->child_count;
  simbol->param_tipos = malloc(sizeof(Tipos) * simbol->num_params);

  for (int i = 0; i < simbol->num_params; i++) {
    simbol->param_tipos[i] = params->childs[i]->info->tVar;
  }

  insertar_simbolo(simbol);
  node->info = simbol;
  node->child_count = 0;
}

void liberar_tabla() {
  for (size_t i = 0; i < global_table->usados; i++)
    free(global_table->tabla[i]);
  free(global_table->tabla);
  free(global_table);
}

void print_symtable() {
  if (global_table == NULL) {
    printf("Symtable not initialized.\n");
    return;
  }

  printf("---- Symtable ----\n");
  printf("Capacity: %zu | Used: %zu\n", global_table->capacidad,
         global_table->usados);

  for (size_t i = 0; i < global_table->usados; i++) {
    Simbolo *s = global_table->tabla[i];
    if (s != NULL) {
      printf("[%zu] Name: %s | Type var: %d | Value: %d\n", i,
             s->nombre ? s->nombre : "(null)", s->tVar, s->valor);
    }
  }
  printf("---------------------------\n");
}
