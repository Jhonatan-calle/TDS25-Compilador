#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdlib.h>
#include "types.h"

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
void print_symtable();

#endif // SYMBOLS_H
