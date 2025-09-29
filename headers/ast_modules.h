#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "forward_declarations.h"
#include "semantic_check.h"

void set_info_value_depending_operator(AST *node, AST *operando1,
                                       AST *operando2, char *op);
void allocate_binary_boolean_node(AST *node, AST *operando1, AST *operando2,
                                  char *op);
void allocate_binary_integer_node(AST *node, AST *operando1, AST *operando2,
                                  char *op);

void module_switch_case_programa(AST *node, va_list args);
void module_switch_case_var_declaration(AST *node, va_list args);
void module_switch_case_method_declaration(AST *node, va_list args);
void module_switch_case_param(AST *node, va_list args);
void module_switch_case_param_list(AST *node, va_list args);
void module_switch_case_block(AST *node, va_list args);
void module_switch_case_asignacion(AST *node, va_list args);
void module_switch_case_invocation(AST *node, va_list args);
void module_switch_case_if(AST *node, va_list args);
void module_switch_case_if_else(AST *node, va_list args);
void module_switch_case_else_cuerpo(AST *node, va_list args);
void module_switch_case_while(AST *node, va_list args);
void module_switch_case_return(AST *node, va_list args);
void module_switch_case_id(AST *node, va_list args);
void module_switch_case_negacion_logica(AST *node, va_list args);
void module_switch_case_negacion_aritmetica(AST *node, va_list args);
void module_switch_case_suma(AST *node, va_list args);
void module_switch_case_resta(AST *node, va_list args);
void module_switch_case_multiplicacion(AST *node, va_list args);
void module_switch_case_divition(AST *node, va_list args);
void module_switch_case_modulo(AST *node, va_list args);
void module_switch_case_less_than(AST *node, va_list args);
void module_switch_case_greater_than(AST *node, va_list args);
void module_switch_case_equal(AST *node, va_list args);
void module_switch_case_and(AST *node, va_list args);
void module_switch_case_or(AST *node, va_list args);
void module_switch_case_literal(AST *node, va_list args);
void module_switch_case_arg_list(AST *node, va_list args);
