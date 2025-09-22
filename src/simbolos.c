#include "../headers/simbolos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Using extern variable defined in main.c
extern TablaSimbolos *global_table;

TablaSimbolos *crear_tabla(size_t capacidad_inicial)
{
  TablaSimbolos *t = malloc(sizeof(TablaSimbolos));
  t->capacidad = capacidad_inicial;
  t->usados = 0;
  t->tabla = malloc(capacidad_inicial * sizeof(Simbolo *));
  return t;
}

void insertar_simbolo(Simbolo *e)
{
  if (global_table->usados >= global_table->capacidad)
  {
    global_table->capacidad *= 2;
    global_table->tabla = realloc(global_table->tabla, global_table->capacidad * sizeof(Simbolo *));
  }
  global_table->tabla[global_table->usados] = e;
  global_table->usados++;
}

Simbolo *buscar_simbolo(char *nombre)
{
  for (size_t i = 0; i < global_table->usados; i++)
    if (strcmp(global_table->tabla[i]->nombre, nombre) == 0)
      return global_table->tabla[i];
  return NULL; // no encontrado
}

void liberar_tabla()
{
  for (size_t i = 0; i < global_table->usados; i++)
    free(global_table->tabla[i]);
  free(global_table->tabla);
  free(global_table);
}
