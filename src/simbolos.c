#include "../headers/simbolos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TablaSimbolos *global_scope_table = NULL;
TablaSimbolos *local_scope_table = NULL;

TablaSimbolos *crear_tabla(size_t capacidad_inicial)
{
  TablaSimbolos *t = malloc(sizeof(TablaSimbolos));
  t->capacidad = capacidad_inicial;
  t->usados = 0;
  t->tabla = malloc(capacidad_inicial * sizeof(Simbolo *));
  return t;
}

void insertar_simbolo(TablaSimbolos* t, Simbolo *e)
{
  if (t->usados >= t->capacidad)
  {
    t->capacidad *= 2;
    t->tabla = realloc(t->tabla, t->capacidad * sizeof(Simbolo *));
  }
  t->tabla[t->usados] = e;
  t->usados++;
}

Simbolo *buscar_simbolo(TablaSimbolos* t, char *nombre)
{
  for (size_t i = 0; i < t->usados; i++)
    if (strcmp(t->tabla[i]->nombre, nombre) == 0)
      return t->tabla[i];
  return NULL; // no encontrado
}

void liberar_tabla(TablaSimbolos* t)
{
  for (size_t i = 0; i < t->usados; i++)
    free(t->tabla[i]);
  free(t->tabla);
  free(t);
}
