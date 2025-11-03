#ifndef FORWARD_DECLARATIONS_H
#define FORWARD_DECLARATIONS_H

typedef struct AST AST;
typedef struct Symbol Symbol;
typedef struct ScopeNode ScopeNode;
typedef struct Scope Scope;
typedef struct TAC TAC;

// Enum for TAC operations
typedef enum {
  TAC_UNKNOWN,
  TAC_ADD,
  TAC_SUB,
  TAC_MUL,
  TAC_DIV,
  TAC_MOD,
  TAC_LESS,
  TAC_LESS_EQ,
  TAC_GR,
  TAC_GREATER_EQ,
  TAC_EQ,
  TAC_AND,
  TAC_OR,
  TAC_ASSIGN,
  TAC_LABEL,
  TAC_LABEL_IF,
  TAC_LABEL_END,
  TAC_GOTO,
  TAC_IFZ, // If zero
  TAC_PARAM,
  TAC_ARG,
  TAC_CALL,
  TAC_RETURN,
  TAC_NOT,
  TAC_NEG,
  TAC_EXTERN
} OpCode;

#endif
