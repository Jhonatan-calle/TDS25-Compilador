#include "../headers/assembly_code.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int temp_counter = 0;

/**
 * Assembly util function
 *
 * Called when a new use of a register is used
 * Uses temp_counter global variable to increment or decrement the amount of
 * registers used
 */
char *new_temp() {
  char buffer[16];
  snprintf(buffer, sizeof(buffer), "t%d", temp_counter++);
  return strdup(buffer);
}

static const char *arg_registers[] = {"%rdi", "%rsi", "%rdx",
                                      "%rcx", "%r8",  "%r9"};

static int next_arg = 0; // índice del próximo registro libre

// Devuelve el siguiente registro de argumento disponible
const char *get_arg_register() {
  static char stack_arg[32]; // buffer para devolver direcciones tipo "16(%rbp)"

  if (next_arg < 6) {
    // Primeros 6 argumentos: registros
    return arg_registers[next_arg++];
  } else {
    // A partir del séptimo argumento: pila
    int stack_offset = 16 + (next_arg - 6) * 8;
    // 16(%rbp) → primer argumento en pila (séptimo total)
    snprintf(stack_arg, sizeof(stack_arg), "%d(%%rbp)", stack_offset);
    next_arg++;
    return stack_arg;
  }
}

// Reinicia el uso de registros de argumentos (después de un call)
void reset_arg_registers() { next_arg = 0; }

/**
 * Assembly util function
 *
 * Called on the top level of the program
 * It constructs and prints a pseudo-assembly recursively
 */
void gen_assembly_code(TAC *head) {
  if (!head) return;

  if (head->op == TAC_ASSIGN)
    printf("section .text:\n");

  for (TAC *t = head; t; t = t->next) {
    switch (t->op) {

    // --- Operaciones aritméticas ---
    case TAC_ADD: {
      // Usamos %r10 como registro temporal (scratch)
      // Soportamos operandos inmediatos (offset == 0) y memoria
      if (t->op1->offset == 0)
        printf("  mov $%d, %%r10\n", t->op1->valor);
      else
        printf("  mov -%d(%%rbp), %%r10\n", t->op1->offset);

      if (t->op2->offset == 0)
        printf("  add $%d, %%r10\n", t->op2->valor);
      else
        printf("  add -%d(%%rbp), %%r10\n", t->op2->offset);

      printf("  mov %%r10, -%d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_SUB: {
      // result = op1 - op2
      if (t->op1->offset == 0)
        printf("  mov $%d, %%r10\n", t->op1->valor);
      else
        printf("  mov -%d(%%rbp), %%r10\n", t->op1->offset);

      if (t->op2->offset == 0)
        printf("  sub $%d, %%r10\n", t->op2->valor);
      else
        printf("  sub -%d(%%rbp), %%r10\n", t->op2->offset);

      printf("  mov %%r10, -%d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_MUL: {
      // imul supports reg, r/m form: imul <r/m64>, <reg>
      if (t->op1->offset == 0)
        printf("  mov $%d, %%r10\n", t->op1->valor);
      else
        printf("  mov -%d(%%rbp), %%r10\n", t->op1->offset);

      if (t->op2->offset == 0)
        printf("  imul $%d, %%r10\n", t->op2->valor); // immediate multiply
      else
        printf("  imul -%d(%%rbp), %%r10\n", t->op2->offset);

      printf("  mov %%r10, -%d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_DIV: {
      // idiv requires dividend in rax, sign-extend in rdx via cqo
      if (t->op1->offset == 0)
        printf("  mov $%d, %%rax\n", t->op1->valor);
      else
        printf("  mov -%d(%%rbp), %%rax\n", t->op1->offset);

      printf("  cqo\n");

      if (t->op2->offset == 0)
        printf("  mov $%d, %%r10\n  idiv %%r10\n", t->op2->valor);
      else
        printf("  idiv -%d(%%rbp)\n", t->op2->offset);

      printf("  mov %%rax, -%d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_MOD: {
      // remainder stored in rdx after idiv
      if (t->op1->offset == 0)
        printf("  mov $%d, %%rax\n", t->op1->valor);
      else
        printf("  mov -%d(%%rbp), %%rax\n", t->op1->offset);

      printf("  cqo\n");

      if (t->op2->offset == 0)
        printf("  mov $%d, %%r10\n  idiv %%r10\n", t->op2->valor);
      else
        printf("  idiv -%d(%%rbp)\n", t->op2->offset);

      printf("  mov %%rdx, -%d(%%rbp)\n", t->result->offset);
      break;
    }

    // --- Comparaciones ---
    case TAC_LESS: {
      // result = op1 < op2
      if (t->op1->offset == 0)
        printf("  mov $%d, %%r10\n", t->op1->valor);
      else
        printf("  mov -%d(%%rbp), %%r10\n", t->op1->offset);

      if (t->op2->offset == 0)
        printf("  cmp $%d, %%r10\n", t->op2->valor);
      else
        printf("  cmp -%d(%%rbp), %%r10\n", t->op2->offset);

      printf("  setl %%al\n");
      printf("  movzbq %%al, %%r10\n");
      printf("  mov %%r10, -%d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_GR: {
      // result = op1 > op2
      if (t->op1->offset == 0)
        printf("  mov $%d, %%r10\n", t->op1->valor);
      else
        printf("  mov -%d(%%rbp), %%r10\n", t->op1->offset);

      if (t->op2->offset == 0)
        printf("  cmp $%d, %%r10\n", t->op2->valor);
      else
        printf("  cmp -%d(%%rbp), %%r10\n", t->op2->offset);

      printf("  setg %%al\n");
      printf("  movzbq %%al, %%r10\n");
      printf("  mov %%r10, -%d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_EQ: {
      // result = op1 == op2
      if (t->op1->offset == 0)
        printf("  mov $%d, %%r10\n", t->op1->valor);
      else
        printf("  mov -%d(%%rbp), %%r10\n", t->op1->offset);

      if (t->op2->offset == 0)
        printf("  cmp $%d, %%r10\n", t->op2->valor);
      else
        printf("  cmp -%d(%%rbp), %%r10\n", t->op2->offset);

      printf("  sete %%al\n");
      printf("  movzbq %%al, %%r10\n");
      printf("  mov %%r10, -%d(%%rbp)\n", t->result->offset);
      break;
    }

    // --- Lógicos ---
    case TAC_AND: {
      // booleanize op1 and op2, then and
      // use r10 and r11 as scratch
      if (t->op1->offset == 0)
        printf("  mov $%d, %%r10\n", t->op1->valor);
      else
        printf("  mov -%d(%%rbp), %%r10\n", t->op1->offset);

      printf("  cmp $0, %%r10\n");
      printf("  setne %%al\n");
      printf("  movzbq %%al, %%r10\n");

      if (t->op2->offset == 0)
        printf("  mov $%d, %%r11\n", t->op2->valor);
      else
        printf("  mov -%d(%%rbp), %%r11\n", t->op2->offset);

      printf("  cmp $0, %%r11\n");
      printf("  setne %%bl\n");
      printf("  and %%bl, %%al\n");
      printf("  movzbq %%al, %%r10\n");
      printf("  mov %%r10, -%d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_OR: {
      // booleanize op1 and op2, then or
      if (t->op1->offset == 0)
        printf("  mov $%d, %%r10\n", t->op1->valor);
      else
        printf("  mov -%d(%%rbp), %%r10\n", t->op1->offset);

      printf("  cmp $0, %%r10\n");
      printf("  setne %%al\n");

      if (t->op2->offset == 0)
        printf("  mov $%d, %%r11\n", t->op2->valor);
      else
        printf("  mov -%d(%%rbp), %%r11\n", t->op2->offset);

      printf("  cmp $0, %%r11\n");
      printf("  setne %%bl\n");
      printf("  or %%bl, %%al\n");
      printf("  movzbq %%al, %%r10\n");
      printf("  mov %%r10, -%d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_NOT: {
      if (t->op1->offset == 0)
        printf("  mov $%d, %%r10\n", t->op1->valor);
      else
        printf("  mov -%d(%%rbp), %%r10\n", t->op1->offset);

      printf("  cmp $0, %%r10\n");
      printf("  sete %%al\n");
      printf("  movzbq %%al, %%r10\n");
      printf("  mov %%r10, -%d(%%rbp)\n", t->result->offset);
      break;
    }

    // --- Unarios ---
    case TAC_NEG: {
      if (t->op1->offset == 0)
        printf("  mov $%d, %%r10\n", t->op1->valor);
      else
        printf("  mov -%d(%%rbp), %%r10\n", t->op1->offset);

      printf("  neg %%r10\n");
      printf("  mov %%r10, -%d(%%rbp)\n", t->result->offset);
      break;
    }

    // --- Asignación ---
    case TAC_ASSIGN: {
      if (t->op1->offset == 0)
        printf("  mov $%d, %%r10\n", t->op1->valor);
      else
        printf("  mov -%d(%%rbp), %%r10\n", t->op1->offset);

      printf("  mov %%r10, -%d(%%rbp)\n", t->result->offset);
      break;
    }

    // --- Control de flujo ---
    case TAC_LABEL:
      printf("%s:\n", t->result->nombre);
      // Si t->result->offset representa cantidad a reservar, se mantiene.
      // Asegurate que ese campo sea el tamaño correcto en tu IR.
      printf("  enter $(8 * %d)\n", t->result->offset);
      break;

    case TAC_GOTO:
      printf("  jmp %s\n", t->result->nombre);
      break;

    case TAC_IFZ:
      if (t->op1->offset == 0)
        printf("  mov $%d, %%r10\n", t->op1->valor);
      else
        printf("  mov -%d(%%rbp), %%r10\n", t->op1->offset);

      printf("  cmp $0, %%r10\n");
      printf("  je %s\n", t->result->nombre);
      break;

    // --- Funciones ---
    case TAC_PARAM: {
      // get_arg_register devuelve "%r??" o "16(%rbp)" (sin '+')
      const char *where = get_arg_register();
      // Si where empieza por '%' -> registro
      if (where[0] == '%') {
        // mover registro al slot local del argumento (en el callee)
        printf("  mov %s, -%d(%%rbp)\n", where, t->op1->offset);
      } else {
        // where es algo como "16(%rbp)" -> mov 16(%rbp), -offset(%rbp)
        printf("  mov %s, -%d(%%rbp)\n", where, t->op1->offset);
      }
      break;
    }

    case TAC_ARG:
      // push desde el slot local del argumento
      printf("  push -%d(%%rbp)\n", t->op1->offset);
      break;

    case TAC_CALL:
      reset_arg_registers();
      printf("  call %s\n", t->result->nombre);
      // t->op1->offset assumed to be count of pushed args (caller responsibility)
      printf("  add $%d, %%rsp\n",
             t->op1 ? t->op1->offset * 8 : 0); // limpiar args
      printf("  mov %%rax, -%d(%%rbp)\n", t->result->offset);
      break;

    case TAC_RETURN:
      reset_arg_registers();

      if (t->result) {
        if (t->result->offset == 0)
          printf("  mov $%d, %%rax\n", t->result->valor);
        else
          printf("  mov -%d(%%rbp), %%rax\n", t->result->offset);
      }

      printf("  leave\n");
      printf("  ret\n");
      break;

    // --- I/O y externos ---
    case TAC_PRINT:
      if (t->op1->offset == 0)
        printf("  mov $%d, %%rdi\n", t->op1->valor);
      else
        printf("  mov -%d(%%rbp), %%rdi\n", t->op1->offset);
      printf("  call print_int\n");
      break;

    case TAC_EXTERN:
      // no se sabía qué extern imprimir; dejar placeholder
      printf("  ; extern placeholder\n");
      break;

    case TAC_UNKNOWN:
    default:
      printf("  ; unknown TAC op %d\n", t->op);
      break;
    }
  }
}
