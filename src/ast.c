#include "../headers/ast.h"
#include "../headers/simbolos.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AST *init_node(TipoNodo type, int child_count)
{
  AST *node = malloc(sizeof(AST));
  if (!node)
  {
    fprintf(stderr, "<<<<<Error: no se pudo reservar memoria para AST>>>>>\n");
    exit(EXIT_FAILURE);
  }

  node->type = type;
  node->info = NULL;
  node->child_count = child_count;
  node->childs = NULL;
  return node;
}

void module_switch_case_programa(AST *node, va_list args)
{
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = va_arg(args, AST *); // $6: lista_sentencias, de tipo AST*
  node->childs[1] = va_arg(args, AST *); // $6: lista_sentencias, de tipo AST*
}

void module_switch_case_var_declaration(AST *node, va_list args)
{
  int tipoIdentificador = va_arg(args, int); // $1: tipos, el enum de los tipos (internamente un int)
  char *nombre = va_arg(args, char *);       // $2: ID, el nombre de la var declarada
  Simbolo *id = buscar_simbolo(nombre);
  if (id)
  {
    fprintf(stderr, "<<<<<Error: identificador '%s' ya declarado>>>>>\n", nombre);
    exit(EXIT_FAILURE);
  }

  AST *exp = va_arg(args, AST *);
  if (exp->info->tVar != id->tVar)
  {
    fprintf(stderr,
            "<<<<<Error semántico: el identificador '%s' es de tipo '%s' "
            "pero se intenta asignar un valor de tipo '%s'>>>>>\n",
            id->nombre, tipoDatoToStr(id->tVar),
            tipoDatoToStr(exp->info->tVar));
    exit(EXIT_FAILURE);
  }
  Simbolo *simbol = malloc(sizeof(Simbolo));
  simbol->tVar = tipoIdentificador; // tipo (enum Tipos)
  simbol->nombre = nombre;          // identificador
  simbol->categoria = S_VAR;
  simbol->valor = exp->info->valor;
  insertar_simbolo(simbol);
  node->info = simbol;
  node->child_count = 0;
}

void module_switch_case_method_declaration(AST *node, va_list args)
{
  int tipoIdentificador = va_arg(args, int); // $1: tipos, el enum de los tipos (internamente un int)
  char *nombre = va_arg(args, char *);       // $2: ID, el nombre de la var declarada

  Simbolo *id = buscar_simbolo(nombre);
  if (id)
  {
    fprintf(stderr, "[Error semántico] Identificador '%s' ya declarado.\n", nombre);
    exit(EXIT_FAILURE);
  }

  AST *params = va_arg(args, AST *);
  Simbolo *simbol = malloc(sizeof(Simbolo));
  simbol->tVar = tipoIdentificador; // tipo (enum Tipos)
  simbol->nombre = nombre;          // identificador
  simbol->categoria = S_FUNC;
  simbol->num_params = params->child_count;
  simbol->param_tipos = malloc(sizeof(Tipos) * simbol->num_params);

  for (int i = 0; i < simbol->num_params; i++)
  {
    simbol->param_tipos[i] = params->childs[i]->info->tVar;
  }

  // TR_METHOD_DECLARATION que no tiene sentencia reservada Extern
  if (node->type == TR_METHOD_DECLARATION)
  {
    AST *cuerpo = va_arg(args, AST *);
    if (tipoIdentificador != T_VOID)
    {
      // accedo a la parte de sentencias en el bloque
      // la cual es el segundo hijo de un bloque
      AST *sentencias = cuerpo->childs[1];

      int sentencesCount = sentencias->child_count;
      int returnFound = 0;

      for (int i = 0; i < sentencesCount; i++)
      {
        AST *sentencia = sentencias->childs[i];

        if (sentencia->type == TR_RETURN)
        {
          returnFound = 1;

          if (sentencia->info->tVar != tipoIdentificador)
          {
            fprintf(stderr,
                    "[Error semántico] En método '%s': "
                    "el 'return' #%d tiene "
                    "tipo '%s', "
                    "se esperaba '%s'.\n",
                    nombre,
                    i + 1,
                    tipoDatoToStr(sentencia->info->tVar),
                    tipoDatoToStr(tipoIdentificador));
            exit(EXIT_FAILURE);
          }

          // Warning de código inalcanzable
          if (i < sentencesCount - 1)
          {
            fprintf(stderr,
                    "[Warning semántico] En método '%s': código después del "
                    "'return' #%d es inalcanzable.\n",
                    nombre, i + 1);
          }
        }
      }

      // Error si no hay return en método no-void
      if (!returnFound)
      {
        fprintf(stderr,
                "[Warning semántico] Método '%s' no tiene un 'return' y es de "
                "tipo no-void.\n",
                nombre);
        exit(EXIT_FAILURE);
      }
    }
  }
  insertar_simbolo(simbol);
  node->info = simbol;
  node->child_count = 0;
}

void module_switch_case_param(AST *node, va_list args)
{
  int tipoIdentificador = va_arg(args, int); // $1: tipos, el enum de los tipos (internamente un int)
  char *nombre = va_arg(args, char *);       // $2: ID, el nombre de la var declarada
  Simbolo *simbol = malloc(sizeof(Simbolo));
  simbol->tVar = tipoIdentificador;
  simbol->nombre = nombre;
  node->info = simbol;
}

void module_switch_case_block(AST *node, va_list args)
{
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = va_arg(args, AST *); // $2: var_declaration_list, de tipo AST*
  node->childs[1] = va_arg(args, AST *); // $3: statement_list, de tipo AST*
}

void module_switch_case_asignacion(AST *node, va_list args)
{
  char *nombre = va_arg(args, char *); // $1: ID, el nombre de la var a asignar
  Simbolo *id = buscar_simbolo(nombre);
  if (!id)
  {
    fprintf(stderr, "<<<<<Error: identificador '%s' no declarado>>>>>\n", nombre);
    exit(EXIT_FAILURE);
  }

  AST *exp = va_arg(args, AST *); // $3: expr, lo que se le va asignar a la variable, de tipo AST*

  if (exp->info->tVar != id->tVar)
  {
    fprintf(stderr,
            "<<<<<Error semántico: el identificador '%s' es de tipo '%s' "
            "pero se intenta asignar un valor de tipo '%s'>>>>>\n",
            id->nombre, tipoDatoToStr(id->tVar),
            tipoDatoToStr(exp->info->tVar));
    exit(EXIT_FAILURE);
  }
  id->valor = exp->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);

  AST *aux = malloc(sizeof(AST));
  aux->type = TR_IDENTIFICADOR;
  aux->info = id;
  aux->child_count = 0;
  aux->childs = NULL;

  node->childs[0] = aux;
  node->childs[1] = exp;
}

void module_switch_case_invocation(AST *node, va_list args)
{
  char *nombre = va_arg(args, char *); // $1: ID, el nombre de la var a asignar
  Simbolo *id = buscar_simbolo(nombre);
  if (!id)
  {
    fprintf(stderr, "[Error semántico] El método '%s' no está declarado.\n", nombre);
    exit(EXIT_FAILURE);
  }

  AST *params = va_arg(args, AST *);
  if (params->child_count != id->num_params)
  {
    fprintf(stderr,
            "[Error semántico] El método '%s' espera %d parámetro(s), "
            "pero se recibieron %d.\n",
            nombre, id->num_params, params->child_count);
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < id->num_params; i++)
  {
    if (id->param_tipos[i] == params->childs[i]->info->tVar)
    {
      fprintf(stderr,
              "[Error semántico] En la llamada a '%s': "
              "el parámetro #%d debería ser de tipo '%s', "
              "pero se encontró '%s'.\n",
              nombre, i + 1,
              tipoDatoToStr(id->param_tipos[i]), // convierte enum Tipo a string
              tipoDatoToStr(params->childs[i]->info->tVar));
      exit(EXIT_FAILURE);
    }
  }
  node->child_count = 1;
  node->info = id;
  node->childs = malloc(sizeof(AST *));
  node->childs[0] = params;
}

void module_switch_case_if(AST *node, va_list args)
{
  AST *condition = va_arg(args, AST *);
  if (condition->info->tVar != T_BOOL)
  {
    fprintf(stderr,
            "[Error semántico] La condición del 'if' debe ser de tipo "
            "'boolean', pero se encontró '%s'.\n",
            tipoDatoToStr(condition->info->tVar));
    exit(EXIT_FAILURE);
  }
  AST *cuerpo = va_arg(args, AST *);

  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = condition;
  node->childs[1] = cuerpo;
}

void module_switch_case_if_else(AST *node, va_list args)
{
  AST *condition = va_arg(args, AST *);
  if (condition->info->tVar != T_BOOL)
  {
    fprintf(stderr,
            "[Error semántico] La condición del 'if' debe ser de tipo "
            "'boolean', pero se encontró '%s'.\n",
            tipoDatoToStr(condition->info->tVar));
    exit(EXIT_FAILURE);
  }
  AST *cuerpo1 = va_arg(args, AST *);
  AST *cuerpo2 = va_arg(args, AST *);

  node->child_count = 3;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = condition;
  node->childs[1] = cuerpo1;
  node->childs[2] = cuerpo2;
}

void module_switch_case_while(AST *node, va_list args)
{
  AST *condition = va_arg(args, AST *);
  if (condition->info->tVar != T_BOOL)
  {
    fprintf(stderr,
            "[Error semántico] La condición del 'while' debe ser de tipo "
            "'boolean', pero se encontró '%s'.\n",
            tipoDatoToStr(condition->info->tVar));
    exit(EXIT_FAILURE);
  }
  AST *cuerpo = va_arg(args, AST *);

  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = condition;
  node->childs[1] = cuerpo;
}

void module_switch_case_return(AST *node, va_list args)
{
  if (node->child_count != 1)
  {
    return;
  }
  node->childs = malloc(sizeof(AST *));
  node->childs[0] = va_arg(args, AST *);
}

void module_switch_case_id(AST *node, va_list args)
{
  if (node->child_count != 1)
  {
    return;
  }
  node->childs = malloc(sizeof(AST *));
  node->childs[0] = va_arg(args, AST *);
}

void module_switch_case_negacion_logica(AST *node, va_list args)
{
  AST *exp = va_arg(args, AST *);
  if (exp->info->tVar != T_BOOL)
  {
    fprintf(stderr,
            "[Error semántico] el operador '!' espera una expresion boleana"
            "pero se encontró '%s'.\n",
            tipoDatoToStr(exp->info->tVar));
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(AST));
  node->info->tVar = T_BOOL;
  node->info->valor = !exp->info->valor;
  node->info->categoria = exp->info->categoria;
  node->child_count = 1;
  node->childs = malloc(sizeof(AST *));
  node->childs[0] = exp;
}

void module_switch_case_negacion_aritmetica(AST *node, va_list args)
{
  AST *exp = va_arg(args, AST *);
  if (exp->info->tVar != T_INT)
  {
    fprintf(stderr,
            "[Error semántico] el operador '-' espera un entero"
            "pero se encontró '%s'.\n",
            tipoDatoToStr(exp->info->tVar));
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(AST));
  node->info->tVar = T_INT;
  node->info->valor = -(exp->info->valor);
  node->child_count = 1;
  node->childs = malloc(sizeof(AST *));
  node->childs[0] = exp;
}

void module_switch_case_suma(AST *node, va_list args)
{
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_INT || operando2->info->tVar != T_INT)
  {
    fprintf(stderr, "[Error semántico] el operador '+' espera un entero");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(AST));
  node->info->tVar = T_INT;
  node->info->valor = operando1->info->valor + operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[0] = operando2;
}

void module_switch_case_resta(AST *node, va_list args)
{
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_INT || operando2->info->tVar != T_INT)
  {
    fprintf(stderr, "[Error semántico] el operador '-' espera un entero");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(AST));
  node->info->tVar = T_INT;
  node->info->valor = operando1->info->valor - operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[0] = operando2;
}

void module_switch_case_multiplicacion(AST *node, va_list args)
{
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_INT || operando2->info->tVar != T_INT)
  {
    fprintf(stderr, "[Error semántico] el operador '*' espera un entero");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(AST));
  node->info->tVar = T_INT;
  node->info->valor = operando1->info->valor * operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[0] = operando2;
}

void module_switch_case_divition(AST *node, va_list args)
{
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_INT || operando2->info->tVar != T_INT)
  {
    fprintf(stderr, "[Error semántico] el operador '/' espera un entero");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(AST));
  node->info->tVar = T_INT;
  node->info->valor = operando1->info->valor / operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[0] = operando2;
}

void module_switch_case_modulo(AST *node, va_list args)
{
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_INT || operando2->info->tVar != T_INT)
  {
    fprintf(stderr, "[Error semántico] el operador '%%' espera un entero");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(AST));
  node->info->tVar = T_INT;
  node->info->valor = operando1->info->valor % operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[0] = operando2;
}

void module_switch_case_less_than(AST *node, va_list args)
{
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_INT || operando2->info->tVar != T_INT)
  {
    fprintf(stderr, "[Error semántico] el operador '<' espera un entero");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(AST));
  node->info->tVar = T_BOOL;
  node->info->valor = operando1->info->valor < operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[0] = operando2;
}

void module_switch_case_greater_than(AST *node, va_list args)
{
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_INT || operando2->info->tVar != T_INT)
  {
    fprintf(stderr, "[Error semántico] el operador '>' espera un entero");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(AST));
  node->info->tVar = T_BOOL;
  node->info->valor = operando1->info->valor > operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[0] = operando2;
}

void module_switch_case_equal(AST *node, va_list args)
{
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_BOOL || operando2->info->tVar != T_BOOL)
  {
    fprintf(stderr, "[Error semántico] el operador '==' espera boleanos");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(AST));
  node->info->tVar = T_BOOL;
  node->info->valor = operando1->info->valor == operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[0] = operando2;
}

void module_switch_case_and(AST *node, va_list args)
{
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_BOOL || operando2->info->tVar != T_BOOL)
  {
    fprintf(stderr, "[Error semántico] el operador '&&' espera boleanos");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(AST));
  node->info->tVar = T_BOOL;
  node->info->valor = operando1->info->valor && operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[0] = operando2;
}

void module_switch_case_or(AST *node, va_list args)
{
  AST *operando1 = va_arg(args, AST *);
  AST *operando2 = va_arg(args, AST *);
  if (operando1->info->tVar != T_BOOL || operando2->info->tVar != T_BOOL)
  {
    fprintf(stderr, "[Error semántico] el operador '||' espera boleanos");
    exit(EXIT_FAILURE);
  }

  node->info = malloc(sizeof(AST));
  node->info->tVar = T_BOOL;
  node->info->valor = operando1->info->valor || operando2->info->valor;
  node->child_count = 2;
  node->childs = malloc(sizeof(AST *) * 2);
  node->childs[0] = operando1;
  node->childs[0] = operando2;
}

void module_switch_case_literal(AST *node, va_list args)
{
  node->info = malloc(sizeof(Simbolo));
  node->info->tVar = va_arg(args, int); // T_INT o T_BOOL, representado internamente como int
  node->info->nombre = strdup("TR_VALOR");
  node->info->valor = va_arg(args, int); // $1 si es valor numerico, 0 si es false o 1 si es true
  node->child_count = 0;
}

AST *new_node(TipoNodo type, int child_count, ...)
{
  AST *node = init_node(type, child_count);

  va_list args;
  va_start(args, child_count);

  switch (type)
  {
  case TR_PROGRAMA:
    module_switch_case_programa(node, args);
    break;
  case TR_VAR_DECLARATION:
    module_switch_case_var_declaration(node, args);
    break;
  case TR_METHOD_DECLARATION:
  case TR_METHOD_DECLARATION_EXTERN:
    module_switch_case_method_declaration(node, args);
    break;
  case TR_PARAM:
    module_switch_case_param(node, args);
    break;
  case TR_BLOCK:
    module_switch_case_block(node, args);
    break;
  case TR_ASIGNACION:
    module_switch_case_asignacion(node, args);
    break;
  case TR_INVOCATION:
    module_switch_case_invocation(node, args);
    break;
  case TR_IF_STATEMENT:
    module_switch_case_if(node, args);
    break;
  case TR_IF_ELSE_STATEMENT:
    module_switch_case_if_else(node, args);
    break;
  case TR_WHILE_STATEMENT:
    module_switch_case_while(node, args);
    break;
  case TR_RETURN:
    module_switch_case_return(node, args);
    break;
  case TR_IDENTIFICADOR:
    module_switch_case_id(node, args);
    break;
  case TR_NEGACION_LOGICA:
    module_switch_case_negacion_logica(node, args);
    break;
  case TR_NEGACION_ARITMETICA:
    module_switch_case_negacion_aritmetica(node, args);
    break;
  case TR_SUMA:
    module_switch_case_suma(node, args);
    break;
  case TR_RESTA:
    module_switch_case_resta(node, args);
    break;
  case TR_MULTIPLICACION:
    module_switch_case_multiplicacion(node, args);
    break;
  case TR_DIVITION:
    module_switch_case_divition(node, args);
    break;
  case TR_MODULO:
    module_switch_case_modulo(node, args);
    break;
  case TR_MENOR:
    module_switch_case_less_than(node, args);
    break;
  case TR_MAYOR:
    module_switch_case_greater_than(node, args);
    break;
  case TR_IGUAL_LOGICO:
    module_switch_case_equal(node, args);
    break;
  case TR_AND:
    module_switch_case_and(node, args);
    break;
  case TR_OR:
    module_switch_case_or(node, args);
    break;
  case TR_VALOR:
    module_switch_case_literal(node, args);
    break;
  default:
    break;
  }

  va_end(args);
  return node;
}

AST *append_child(AST *list, AST *child)
{

  list->childs = realloc(list->childs, sizeof(AST *) * (list->child_count + 1));
  if (!list->childs)
  {
    fprintf(stderr, "Error realloc en append_child\n");
    exit(EXIT_FAILURE);
  }
  list->childs[list->child_count] = child;
  list->child_count += 1;

  return list;
}

void free_ast(AST *node)
{
  if (!node)
    return;

  for (int i = 0; i < node->child_count; i++)
    free_ast(node->childs[i]);
  free(node->childs);
  free(node);
}

const char *tipoDatoToStr(Tipos type)
{
  switch (type)
  {
  case T_INT:
    return "INT";
  case T_BOOL:
    return "BOOL";
  case T_VOID:
    return "VOID";
  default:
    return "DESCONOCIDO";
  }
}
