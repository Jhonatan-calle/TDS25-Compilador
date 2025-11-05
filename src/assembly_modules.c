#include "../headers/assembly_modules.h"

void assembly_module_add(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  // We use %r10 as a temporal register (scratch)
  // We support immediate operands (offset == 0) and memory
  if (t->op1->offset == 0)
    fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
  else
    fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

  if (t->op2->offset == 0)
    fprintf(asm_out, "  add $%d, %%r10\n", t->op2->value);
  else
    fprintf(asm_out, "  add %d(%%rbp), %%r10\n", t->op2->offset);

  fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
}

void assembly_module_sub(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  // result = op1 - op2
  if (t->op1->offset == 0)
    fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
  else
    fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

  if (t->op2->offset == 0)
    fprintf(asm_out, "  sub $%d, %%r10\n", t->op2->value);
  else
    fprintf(asm_out, "  sub %d(%%rbp), %%r10\n", t->op2->offset);

  fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
}

void assembly_module_mul(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  // imul supports reg, r/m form: imul <r/m64>, <reg>
  if (t->op1->offset == 0)
    fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
  else
    fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

  if (t->op2->offset == 0)
    fprintf(asm_out, "  imul $%d, %%r10\n",
            t->op2->value); // immediate multiply
  else
    fprintf(asm_out, "  imul %d(%%rbp), %%r10\n", t->op2->offset);

  fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
}

void assembly_module_div(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  // idiv requires dividend in rax, sign-extend in rdx via cqo
  if (t->op1->offset == 0)
    fprintf(asm_out, "  mov $%d, %%rax\n", t->op1->value);
  else
    fprintf(asm_out, "  mov %d(%%rbp), %%rax\n", t->op1->offset);

  fprintf(asm_out, "  cqo\n");

  if (t->op2->offset == 0)
    fprintf(asm_out, "  mov $%d, %%r10\n  idiv %%r10\n", t->op2->value);
  else
    fprintf(asm_out, "  idiv %d(%%rbp)\n", t->op2->offset);

  fprintf(asm_out, "  mov %%rax, %d(%%rbp)\n", t->result->offset);
}

void assembly_module_mod(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  // remainder stored in rdx after idiv
  if (t->op1->offset == 0)
    fprintf(asm_out, "  mov $%d, %%rax\n", t->op1->value);
  else
    fprintf(asm_out, "  mov %d(%%rbp), %%rax\n", t->op1->offset);

  fprintf(asm_out, "  cqo\n");

  if (t->op2->offset == 0)
    fprintf(asm_out, "  mov $%d, %%r10\n  idiv %%r10\n", t->op2->value);
  else
    fprintf(asm_out, "  idiv %d(%%rbp)\n", t->op2->offset);

  fprintf(asm_out, "  mov %%rdx, %d(%%rbp)\n", t->result->offset);
}

void assembly_module_less(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  // result = op1 < op2
  if (t->op1->offset == 0)
    fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
  else
    fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

  if (t->op2->offset == 0)
    fprintf(asm_out, "  cmp $%d, %%r10\n", t->op2->value);
  else
    fprintf(asm_out, "  cmp %d(%%rbp), %%r10\n", t->op2->offset);

  fprintf(asm_out, "  setl %%al\n");
  fprintf(asm_out, "  movzbq %%al, %%r10\n");
  fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
}

void assembly_module_less_eq(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  // result = op1 <= op2
  if (t->op1->offset == 0)
    fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
  else
    fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

  if (t->op2->offset == 0)
    fprintf(asm_out, "  cmp $%d, %%r10\n", t->op2->value);
  else
    fprintf(asm_out, "  cmp %d(%%rbp), %%r10\n", t->op2->offset);

  fprintf(asm_out, "  setle %%al\n"); // set if less or equal (signed)
  fprintf(asm_out, "  movzbq %%al, %%r10\n");
  fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
}

void assembly_module_gr(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  // result = op1 > op2
  if (t->op1->offset == 0)
    fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
  else
    fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

  if (t->op2->offset == 0)
    fprintf(asm_out, "  cmp $%d, %%r10\n", t->op2->value);
  else
    fprintf(asm_out, "  cmp %d(%%rbp), %%r10\n", t->op2->offset);

  fprintf(asm_out, "  setg %%al\n");
  fprintf(asm_out, "  movzbq %%al, %%r10\n");
  fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
}

void assembly_module_greater_eq(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  // result = op1 >= op2
  if (t->op1->offset == 0)
    fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
  else
    fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

  if (t->op2->offset == 0)
    fprintf(asm_out, "  cmp $%d, %%r10\n", t->op2->value);
  else
    fprintf(asm_out, "  cmp %d(%%rbp), %%r10\n", t->op2->offset);

  fprintf(asm_out, "  setge %%al\n"); // set if greater or equal (signed)
  fprintf(asm_out, "  movzbq %%al, %%r10\n");
  fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
}

void assembly_module_eq(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  // result = op1 == op2
  if (t->op1->offset == 0)
    fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
  else
    fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

  if (t->op2->offset == 0)
    fprintf(asm_out, "  mov $%d, %%r11\n", t->op2->value);
  else
    fprintf(asm_out, "  mov %d(%%rbp), %%r11\n", t->op2->offset);

  fprintf(asm_out, "  cmp %%r10, %%r11\n");
  fprintf(asm_out, "  mov $0, %%r11\n");
  fprintf(asm_out, "  mov $1, %%r10\n");
  fprintf(asm_out, "  cmove %%r10, %%r11\n");
  fprintf(asm_out, "  mov %%r11, %d(%%rbp)\n", t->result->offset);
}

void assembly_module_and(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  // booleanize op1 and op2, then and
  // use r10 and r11 as scratch
  if (t->op1->offset == 0)
    fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
  else
    fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

  fprintf(asm_out, "  cmp $0, %%r10\n");
  fprintf(asm_out, "  setne %%al\n");
  fprintf(asm_out, "  movzbq %%al, %%r10\n");

  if (t->op2->offset == 0)
    fprintf(asm_out, "  mov $%d, %%r11\n", t->op2->value);
  else
    fprintf(asm_out, "  mov %d(%%rbp), %%r11\n", t->op2->offset);

  fprintf(asm_out, "  cmp $0, %%r11\n");
  fprintf(asm_out, "  setne %%bl\n");
  fprintf(asm_out, "  and %%bl, %%al\n");
  fprintf(asm_out, "  movzbq %%al, %%r10\n");
  fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
}

void assembly_module_or(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  // booleanize op1 and op2, then or
  if (t->op1->offset == 0)
    fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
  else
    fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

  fprintf(asm_out, "  cmp $0, %%r10\n");
  fprintf(asm_out, "  setne %%al\n");

  if (t->op2->offset == 0)
    fprintf(asm_out, "  mov $%d, %%r11\n", t->op2->value);
  else
    fprintf(asm_out, "  mov %d(%%rbp), %%r11\n", t->op2->offset);

  fprintf(asm_out, "  cmp $0, %%r11\n");
  fprintf(asm_out, "  setne %%bl\n");
  fprintf(asm_out, "  or %%bl, %%al\n");
  fprintf(asm_out, "  movzbq %%al, %%r10\n");
  fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
}

void assembly_module_not(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  if (t->op1->offset == 0)
    fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
  else
    fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

  fprintf(asm_out, "  cmp $0, %%r10\n");
  fprintf(asm_out, "  sete %%al\n");
  fprintf(asm_out, "  movzbq %%al, %%r10\n");
  fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
}

void assembly_module_neg(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  if (t->op1->offset == 0)
    fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
  else
    fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

  fprintf(asm_out, "  neg %%r10\n");
  fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
}

void assembly_module_assign(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  if (t->op1->offset == 0)
    fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
  else
    fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

  fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
}

void assembly_module_label(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  fprintf(asm_out, "  .globl %s\n", t->result->name);
  fprintf(asm_out, "%s:\n", t->result->name);
  // If t->result->offset represents amount to reserve, it keeps.
  fprintf(asm_out, "  enter $(8 * %d), $0\n", t->result->offset);
}

void assembly_module_label_if(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  fprintf(asm_out, "%s:\n", t->result->name);
}

void assembly_module_goto(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  fprintf(asm_out, "  jmp %s\n", t->result->name);
}

void assembly_module_ifz(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  if (t->op1->offset == 0)
    fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->value);
  else
    fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

  fprintf(asm_out, "  mov $1, %%r11\n");
  fprintf(asm_out, "  cmp %%r10, %%r11\n");
  fprintf(asm_out, "  jne %s\n", t->result->name);
}

void assembly_module_param(FILE *asm_out, TAC *t) {
  const char *where = get_arg_register();
  if (where[0] == '%') {
    // Register --> memory (valid)
    fprintf(asm_out, "  mov %s, %d(%%rbp)\n", where, t->op1->offset);
  } else {
    // Memory --> memory (not allowed) --> go through a temp register
    fprintf(asm_out, "  mov %s, %%r10\n", where);
    fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->op1->offset);
  }
}

void assembly_module_arg(FILE *asm_out, TAC *t) {
  // Prepare argument (registers or stack) --> use get_arg_register() once
  const char *where = get_arg_register();
  if (t->op1->offset == 0) {
    // Immediate
    if (where[0] == '%')
      fprintf(asm_out, "  mov $%d, %s\n", t->op1->value, where);
    else
      fprintf(asm_out, "  movq $%d, %s\n", t->op1->value, where);
  } else {
    // Value at local memory/temporal
    if (where[0] == '%') {
      fprintf(asm_out, "  mov %d(%%rbp), %s\n", t->op1->offset, where);
    } else {
      // Memory --> memory is not allowed; go through a temp register
      fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);
      fprintf(asm_out, "  movq %%r10, %s\n", where);
    }
  }
}

void assembly_module_call(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  fprintf(asm_out, "  call %s\n", t->op1->name);
  // t->op1->offset assumed to be count of pushed args (caller
  // responsibility)
  if (t->result)
    fprintf(asm_out, "  mov %%rax, %d(%%rbp)\n", t->result->offset);
}

void assembly_module_return(FILE *asm_out, TAC *t) {
  reset_arg_registers();
  if (t->result) {
    if (t->result->offset == 0)
      fprintf(asm_out, "  mov $%d, %%rax\n", t->result->value);
    else
      fprintf(asm_out, "  mov %d(%%rbp), %%rax\n", t->result->offset);
  }
  fprintf(asm_out, "  leave\n");
  fprintf(asm_out, "  ret\n");
}

void assembly_module_extern(FILE *asm_out, TAC *t) {
  // Pass
  // No senteces in assembly for "extern" keyword.
  // Only in the call of it.
}
