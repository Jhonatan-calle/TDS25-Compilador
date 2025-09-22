#ifndef SIMBOLOS_H
#define SIMBOLOS_H

#include "tipos.h"
#include <stdlib.h>

typedef struct AST AST;


typedef struct {
  Categoria categoria;
  char *nombre; // identificador
  Tipos tVar;   // tipo
  int valor;    // valor ---SOLO VARIABLES
  //-----SOLO METODOS
  int num_params;
  Tipos *param_tipos;
  AST *cuerpo;
} Simbolo;

typedef struct {
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
