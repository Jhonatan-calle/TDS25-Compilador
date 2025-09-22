#ifndef SIMBOLOS_H
#define SIMBOLOS_H

#include "tipos.h"
#include "forward_declarations.h"
#include <stdlib.h>

struct Simbolo
{
  Categoria categoria;
  char *nombre; // identificador
  Tipos tVar;   // tipo
  int valor;    // valor ---SOLO VARIABLES
  //-----SOLO METODOS
  int num_params;
  Tipos *param_tipos;
  AST *cuerpo;
};

typedef struct
{
  Simbolo **tabla;
  size_t capacidad;
  size_t usados;
} TablaSimbolos;

extern TablaSimbolos *global_table;

TablaSimbolos *crear_tabla(size_t capacidad_inicial);
void insertar_simbolo(Simbolo *e);
Simbolo *buscar_simbolo(char *nombre);
void liberar_tabla();

#endif // SIMBOLOS_H
