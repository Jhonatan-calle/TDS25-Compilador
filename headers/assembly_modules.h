#include "ast.h"
#include "symbols.h"
#include "three_address_code.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void assembly_module_add(FILE *asm_out, TAC *t);
void assembly_module_sub(FILE *asm_out, TAC *t);
void assembly_module_mul(FILE *asm_out, TAC *t);
void assembly_module_div(FILE *asm_out, TAC *t);
void assembly_module_mod(FILE *asm_out, TAC *t);
void assembly_module_less(FILE *asm_out, TAC *t);
void assembly_module_less_eq(FILE *asm_out, TAC *t);
void assembly_module_gr(FILE *asm_out, TAC *t);
void assembly_module_greater_eq(FILE *asm_out, TAC *t);
void assembly_module_eq(FILE *asm_out, TAC *t);
void assembly_module_and(FILE *asm_out, TAC *t);
void assembly_module_or(FILE *asm_out, TAC *t);
void assembly_module_not(FILE *asm_out, TAC *t);
void assembly_module_neg(FILE *asm_out, TAC *t);
void assembly_module_assign(FILE *asm_out, TAC *t);
void assembly_module_label(FILE *asm_out, TAC *t);
void assembly_module_label_if(FILE *asm_out, TAC *t);
void assembly_module_goto(FILE *asm_out, TAC *t);
void assembly_module_ifz(FILE *asm_out, TAC *t);
void assembly_module_param(FILE *asm_out, TAC *t);
void assembly_module_arg(FILE *asm_out, TAC *t);
void assembly_module_call(FILE *asm_out, TAC *t);
void assembly_module_return(FILE *asm_out, TAC *t);
void assembly_module_extern(FILE *asm_out, TAC *t);
