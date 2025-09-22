#ifndef SIMBOLOS_H
#define SIMBOLOS_H

#include "tipos.h"
#include "ast.h"
#include <stdlib.h>

typedef struct
{
  Categoria categoria;
  char *nombre; // identificador
  Tipos tVar;   // tipo
  int valor;    // valor ---SOLO VARIABLES
  //-----SOLO METODOS
  int num_params;
  Tipos *param_tipos;
  AST *cuerpo;
} Simbolo;

typedef struct
{
  Simbolo **tabla;
  size_t capacidad;
  size_t usados;
} TablaSimbolos;

extern TablaSimbolos *global_scope_table;
extern TablaSimbolos *local_scope_table;

TablaSimbolos *crear_tabla(size_t capacidad_inicial);
void insertar_simbolo(TablaSimbolos* t, Simbolo *e);
Simbolo *buscar_simbolo(TablaSimbolos* t, char *nombre);
void liberar_tabla(TablaSimbolos* t);

#endif // SIMBOLOS_H
