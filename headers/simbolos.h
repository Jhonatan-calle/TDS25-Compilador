#ifndef SIMBOLOS_H
#define SIMBOLOS_H

#include <stdlib.h>
#include "tipos.h"

typedef struct
{
  char *nombre; // identificador
  Tipos tVar;   // tipo de variable
  int valor;    // valor
} Simbolo;

typedef struct
{
  Simbolo **tabla;
  size_t capacidad;
  size_t usados;
} TablaSimbolos;

extern TablaSimbolos *t;

TablaSimbolos *crear_tabla(size_t capacidad_inicial);
void insertar_simbolo(Simbolo *e);
Simbolo *buscar_simbolo(char *nombre);
void liberar_tabla();

#endif // SIMBOLOS_H
