#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdlib.h>

#include "types.h"
#include "forward_declarations.h"

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
void print_symtable();

#endif // SYMBOLS_H
