#ifndef TYPES_H
#define TYPES_H

typedef enum {
  TR_PROGRAM,
  TR_METHOD_DECLARATION,
  TR_EXTERN,
  TR_VAR_DECLARATION,
  TR_PARAM,
  TR_PARAM_LIST,
  TR_BLOCK,
  TR_ASSIGN,
  TR_INVOCATION,
  TR_IF_STATEMENT,
  TR_ELSE_BODY,
  TR_WHILE_STATEMENT,
  TR_RETURN,
  TR_LOGIC_NEGATION,
  TR_ARITHMETIC_NEGATION,
  TR_ADDITION,
  TR_SUBSTRACTION,
  TR_MULTIPLICATION,
  TR_DIVITION,
  TR_MODULO,
  TR_LESS_THAN,
  TR_GREATER_THAN,
  TR_LOGIC_EQUAL,
  TR_AND,
  TR_OR,
  TR_IDENTIFIER,
  TR_VALUE,
  TR_ARG_LIST,

  TR_DECLARATION_LIST,
  TR_SENTENCES_LIST
} TipoNodo;

typedef enum { T_INT, T_BOOL, T_VOID } Tipos;

typedef enum { S_VAR, S_FUNC } MethoCategory;

typedef enum {
    /* === Asignación y movimiento === */
    TAC_ASSIGN,      // x = y
    TAC_COPY,        // x = y (alias más explícito, si querés separar)

    /* === Operaciones aritméticas === */
    TAC_ADD,         // x = y + z
    TAC_SUB,         // x = y - z
    TAC_MUL,         // x = y * z
    TAC_DIV,         // x = y / z
    TAC_MOD,         // x = y % z
    TAC_NEG,         // x = -y (unario)

    /* === Operaciones lógicas / booleanas === */
    TAC_NOT,         // x = !y
    TAC_AND,         // x = y && z
    TAC_OR,          // x = y || z

    /* === Comparaciones === */
    TAC_LT,          // x = (y < z)
    TAC_GT,          // x = (y > z)
    TAC_EQ,          // x = (y == z)

    /* === Control de flujo === */
    TAC_GOTO,        // goto L
    TAC_IFZ,         // if x == 0 goto L   (salto condicional)
    TAC_IFNZ,        // if x != 0 goto L   (salto condicional)
    TAC_LABEL,       // etiqueta L:

    /* === Funciones === */
    TAC_PARAM,       // param x
    TAC_CALL,        // call f, n_args
    TAC_RETURN,      // return x

} TAC_Op;


#endif // TYPES_H
