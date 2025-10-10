#include "ast.h"
#include "types.h"

void exit_if_already_declared(char *nombre);
void exit_if_already_declared_locally(char *nombre);
void exit_if_not_declared(char *nombre);
void exit_if_types_invalid_at_declaration(AST *exp, Tipos tipoIdentificador, char *nombre);
void exit_if_invalid_return_type(AST *sentencia, int tipoIdentificador,
                                 char *nombre, int i);
void warning_if_unreachable_code(int i, int sentencesCount, char *nombre);
void exit_if_no_return_in_non_void_method(int returnFound, char *nombre);
void exit_if_invalid_types_at_assignment(AST *exp, Simbolo *id);
void exit_if_invalid_amount_of_params(AST *params, Simbolo *id, char *nombre);
void exit_if_missmatch_types_params_at_invocation(AST *params, Simbolo *id,
                                                  char *nombre, int i);
void exit_if_invalid_predicate_type(AST *condition);
void exit_if_unary_arithmetic_operator_mismatch_types(AST *exp, char *op);
void exit_if_binary_arithmetic_operator_mismatch_types(AST *operando1,
                                                       AST *operando2,
                                                       char *op);
void exit_if_unary_boolean_operator_mismatch_types(AST *exp, char *op);
void exit_if_binary_boolean_operator_mismatch_types(AST *operando1,
                                                    AST *operando2, char *op);
void exit_if_operators_mismatch_types(AST *operando1, AST *operando2, char *op);
void exit_if_return_with_no_expression(AST *sentencia, char *nombre, int i);
