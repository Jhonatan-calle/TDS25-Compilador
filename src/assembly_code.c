#include "../headers/assembly_code.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global input filename provided by main/utils; used as output target here
char *ctds_filename = "last_generated_assembly.s";

// Local file handle to write assembly; fallback to stdout if not set
static FILE *asm_out = NULL;

int temp_counter = 0;

int local_slots = 0; // número de variables/params (slots)
int temp_slots = 0;  // número de temporales (slots)

void enter_function() {
  local_slots = 0;
  temp_slots = 0;
}

int alloc_local() {
  local_slots += 1;          // una nueva ranura de 8 bytes
  return -(local_slots * 8); // -8, -16, -24, ...
}

/* reserva un temporal *después* de las locales, devuelve offset negativo único
 */
int alloc_temp() {
  temp_slots += 1;
  /* ubicamos el temporal justo después de las locales:
     offset = -((local_slots + temp_index) * 8) */
  return -((local_slots + temp_slots) * 8);
}

void gen_offsets(AST *root) {
  if (!root)
    return;

  switch (root->type) {

  case TR_PROGRAM:
    // recorrer hijos
    for (int i = 0; i < root->child_count; i++)
      gen_offsets(root->childs[i]);
    break;

  case TR_METHOD_DECLARATION:
    enter_function();

    // Procesar parámetros primero
    for (int i = 0; root->childs[0] && i < root->childs[0]->child_count; i++) {
      AST *param = root->childs[0]->childs[i];
      param->info->offset = alloc_local();
    }

    // Ahora procesar el cuerpo de la función
    gen_offsets(root->childs[1]);

    root->info->offset = (local_slots + temp_slots); // tamaño final del frame
    break;

  case TR_VAR_DECLARATION:
    root->info->offset = alloc_local();
    gen_offsets(root->childs[0]); // inicializador
    break;

  case TR_ASSIGN:
    gen_offsets(root->childs[1]);
    root->info->offset = root->childs[0]->info->offset;
    break;

  case TR_VALUE:
    root->info->offset = 0;
    break;

  case TR_INVOCATION:
    // Reservar temporales para cada arg
    if (root->child_count > 0)
      gen_offsets(root->childs[0]);
    root->info->offset = alloc_temp();
    break;

  case TR_ARG_LIST:
    for (int i = 0; i < root->child_count; i++)
      gen_offsets(root->childs[i]);
    break;

  case TR_ADDITION:
  case TR_SUBSTRACTION:
  case TR_MULTIPLICATION:
  case TR_DIVITION:
  case TR_MODULO:
  case TR_LESS_THAN:
  case TR_GREATER_THAN:
  case TR_LOGIC_EQUAL:
  case TR_AND:
  case TR_OR:
  case TR_LOGIC_NEGATION:
  case TR_ARITHMETIC_NEGATION:
    // hijos primero
    gen_offsets(root->childs[0]);
    if (root->child_count > 1)
      gen_offsets(root->childs[1]);
    root->info->offset = alloc_temp();
    break;

  case TR_IF_STATEMENT:
  case TR_WHILE_STATEMENT:
  case TR_RETURN:
  case TR_BLOCK:
  case TR_SENTENCES_LIST:
  case TR_ELSE_BODY:
    for (int i = 0; i < root->child_count; i++)
      gen_offsets(root->childs[i]);
    break;

  default:
    // fallback safe
    for (int i = 0; i < root->child_count; i++)
      gen_offsets(root->childs[i]);
    break;
  }
}

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
  if (!head)
    return;

  // Open (or switch) output to the file indicated by ctds_filename.
  // If unavailable or failing to open, we fallback to stdout.
  if (ctds_filename && *ctds_filename) {
    asm_out = fopen(ctds_filename, "w");
    if (!asm_out) {
      perror("Error opening assembly output file");
    }
  }

  bool in_function = false;

  fprintf(asm_out, "  .data");
  TAC *q;

  for (q = head; q && !in_function; q = q->next) {
    if (q->op == TAC_ASSIGN) {
      // Si el resultado es global (sin función activa)
      fprintf(asm_out, "%s:\n", q->result->nombre);
      fprintf(asm_out, "  .quad %d\n", q->op1 ? q->op1->valor : 0);
    }

    if (q->op == TAC_LABEL) {
      // cuando empieza una función
      in_function = true;
      break;
    }
  }

  fprintf(asm_out, "\n  .text\n");

  for (TAC *t = q; t; t = t->next) {
    switch (t->op) {

    // --- Operaciones aritméticas ---
    case TAC_ADD: {
      reset_arg_registers();
      // Usamos %r10 como registro temporal (scratch)
      // Soportamos operandos inmediatos (offset == 0) y memoria
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->valor);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      if (t->op2->offset == 0)
        fprintf(asm_out, "  add $%d, %%r10\n", t->op2->valor);
      else
        fprintf(asm_out, "  add %d(%%rbp), %%r10\n", t->op2->offset);

      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_SUB: {
      reset_arg_registers();
      // result = op1 - op2
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->valor);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      if (t->op2->offset == 0)
        fprintf(asm_out, "  sub $%d, %%r10\n", t->op2->valor);
      else
        fprintf(asm_out, "  sub %d(%%rbp), %%r10\n", t->op2->offset);

      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_MUL: {
      reset_arg_registers();
      // imul supports reg, r/m form: imul <r/m64>, <reg>
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->valor);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      if (t->op2->offset == 0)
        fprintf(asm_out, "  imul $%d, %%r10\n",
                t->op2->valor); // immediate multiply
      else
        fprintf(asm_out, "  imul %d(%%rbp), %%r10\n", t->op2->offset);

      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_DIV: {
      reset_arg_registers();
      // idiv requires dividend in rax, sign-extend in rdx via cqo
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%rax\n", t->op1->valor);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%rax\n", t->op1->offset);

      fprintf(asm_out, "  cqo\n");

      if (t->op2->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n  idiv %%r10\n", t->op2->valor);
      else
        fprintf(asm_out, "  idiv %d(%%rbp)\n", t->op2->offset);

      fprintf(asm_out, "  mov %%rax, %d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_MOD: {
      reset_arg_registers();
      // remainder stored in rdx after idiv
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%rax\n", t->op1->valor);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%rax\n", t->op1->offset);

      fprintf(asm_out, "  cqo\n");

      if (t->op2->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n  idiv %%r10\n", t->op2->valor);
      else
        fprintf(asm_out, "  idiv %d(%%rbp)\n", t->op2->offset);

      fprintf(asm_out, "  mov %%rdx, %d(%%rbp)\n", t->result->offset);
      break;
    }

    // --- Comparaciones ---
    case TAC_LESS: {
      reset_arg_registers();
      // result = op1 < op2
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->valor);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      if (t->op2->offset == 0)
        fprintf(asm_out, "  cmp $%d, %%r10\n", t->op2->valor);
      else
        fprintf(asm_out, "  cmp %d(%%rbp), %%r10\n", t->op2->offset);

      fprintf(asm_out, "  setl %%al\n");
      fprintf(asm_out, "  movzbq %%al, %%r10\n");
      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_GR: {
      reset_arg_registers();
      // result = op1 > op2
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->valor);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      if (t->op2->offset == 0)
        fprintf(asm_out, "  cmp $%d, %%r10\n", t->op2->valor);
      else
        fprintf(asm_out, "  cmp %d(%%rbp), %%r10\n", t->op2->offset);

      fprintf(asm_out, "  setg %%al\n");
      fprintf(asm_out, "  movzbq %%al, %%r10\n");
      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_EQ: {
      reset_arg_registers();
      // result = op1 == op2
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->valor);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      if (t->op2->offset == 0)
        fprintf(asm_out, "  cmp $%d, %%r10\n", t->op2->valor);
      else
        fprintf(asm_out, "  cmp %d(%%rbp), %%r10\n", t->op2->offset);

      fprintf(asm_out, "  mov $0, %%r11\n");
      fprintf(asm_out, "  mov $1, %%r10\n");
      fprintf(asm_out, "  cmp %%r10, %%r11\n");
      fprintf(asm_out, "  mov %%r11, %d(%%rbp)\n", t->result->offset);
      break;
    }

    // --- Lógicos ---
    case TAC_AND: {
      reset_arg_registers();
      // booleanize op1 and op2, then and
      // use r10 and r11 as scratch
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->valor);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      fprintf(asm_out, "  cmp $0, %%r10\n");
      fprintf(asm_out, "  setne %%al\n");
      fprintf(asm_out, "  movzbq %%al, %%r10\n");

      if (t->op2->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r11\n", t->op2->valor);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r11\n", t->op2->offset);

      fprintf(asm_out, "  cmp $0, %%r11\n");
      fprintf(asm_out, "  setne %%bl\n");
      fprintf(asm_out, "  and %%bl, %%al\n");
      fprintf(asm_out, "  movzbq %%al, %%r10\n");
      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_OR: {
      reset_arg_registers();
      // booleanize op1 and op2, then or
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->valor);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      fprintf(asm_out, "  cmp $0, %%r10\n");
      fprintf(asm_out, "  setne %%al\n");

      if (t->op2->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r11\n", t->op2->valor);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r11\n", t->op2->offset);

      fprintf(asm_out, "  cmp $0, %%r11\n");
      fprintf(asm_out, "  setne %%bl\n");
      fprintf(asm_out, "  or %%bl, %%al\n");
      fprintf(asm_out, "  movzbq %%al, %%r10\n");
      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    case TAC_NOT: {
      reset_arg_registers();
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->valor);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      fprintf(asm_out, "  cmp $0, %%r10\n");
      fprintf(asm_out, "  sete %%al\n");
      fprintf(asm_out, "  movzbq %%al, %%r10\n");
      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    // --- Unarios ---
    case TAC_NEG: {
      reset_arg_registers();
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->valor);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      fprintf(asm_out, "  neg %%r10\n");
      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    // --- Asignación ---
    case TAC_ASSIGN: {
      reset_arg_registers();
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->valor);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);

      fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->result->offset);
      break;
    }

    // --- Control de flujo ---
    case TAC_LABEL:
      reset_arg_registers();
      fprintf(asm_out, "  .globl %s\n", t->result->nombre);
      fprintf(asm_out, "%s:\n", t->result->nombre);
      // Si t->result->offset representa cantidad a reservar, se mantiene.
      fprintf(asm_out, "  enter $(8 * %d), $0\n", -t->result->offset);
      break;

    case TAC_LABEL_END:
    case TAC_LABEL_IF:
      reset_arg_registers();
      fprintf(asm_out, "%s:\n", t->result->nombre);
      break;

    case TAC_GOTO:
      reset_arg_registers();
      fprintf(asm_out, "  jmp %s\n", t->result->nombre);
      break;

    case TAC_IFZ:
      reset_arg_registers();
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%r10\n", t->op1->valor);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%r11\n", t->op1->offset);

      fprintf(asm_out, "  cmp %%r10, %%r11\n");
      fprintf(asm_out, "  je %s\n", t->result->nombre);
      break;

    // --- Funciones ---
    case TAC_PARAM: {
      const char *where = get_arg_register();
      if (where[0] == '%') {
        // Registro → memoria (válido)
        fprintf(asm_out, "  mov %s, %d(%%rbp)\n", where, t->op1->offset);
      } else {
        // Memoria → memoria (no permitido) → pasar por registro temporal
        fprintf(asm_out, "  mov %s, %%r10\n", where);
        fprintf(asm_out, "  mov %%r10, %d(%%rbp)\n", t->op1->offset);
      }
      break;
    }

    case TAC_ARG: {
      // preparar argumento (registros o stack) -> usar get_arg_register() UNA
      // vez
      const char *where = get_arg_register();
      if (t->op1->offset == 0) {
        /* inmediato */
        if (where[0] == '%')
          fprintf(asm_out, "  mov $%d, %s\n", t->op1->valor, where);
        else
          fprintf(asm_out, "  movq $%d, %s\n", t->op1->valor, where);
      } else {
        /* valor en memoria local/temporal */
        if (where[0] == '%') {
          fprintf(asm_out, "  mov %d(%%rbp), %s\n", t->op1->offset, where);
        } else {
          /* memoria -> memoria no es permitido; usar registro temporal */
          fprintf(asm_out, "  mov %d(%%rbp), %%r10\n", t->op1->offset);
          fprintf(asm_out, "  movq %%r10, %s\n", where);
        }
      }
      break;
    }

    case TAC_CALL:
      reset_arg_registers();
      fprintf(asm_out, "  call %s\n", t->op1->nombre);
      // t->op1->offset assumed to be count of pushed args (caller
      // responsibility)
      if (t->result)
        fprintf(asm_out, "  mov %%rax, %d(%%rbp)\n", t->result->offset);
      break;

    case TAC_RETURN:
      reset_arg_registers();

      if (t->result) {
        if (t->result->offset == 0)
          fprintf(asm_out, "  mov $%d, %%rax\n", t->result->valor);
        else
          fprintf(asm_out, "  mov %d(%%rbp), %%rax\n", t->result->offset);
      }

      break;

    // --- I/O y externos ---
    case TAC_PRINT:
      if (t->op1->offset == 0)
        fprintf(asm_out, "  mov $%d, %%rdi\n", t->op1->valor);
      else
        fprintf(asm_out, "  mov %d(%%rbp), %%rdi\n", t->op1->offset);
      fprintf(asm_out, "  call print_int\n");
      break;

    case TAC_EXTERN:
      // no se sabía qué extern imprimir; dejar placeholder
      break;

    case TAC_UNKNOWN:
    default:
      fprintf(asm_out, "  ; unknown TAC op %d\n", t->op);
      break;
    }
  }
  fprintf(asm_out, "  leave\n");
  fprintf(asm_out, "  ret\n");
  fprintf(asm_out, "  .section  .note.GNU-stack,\"\",@progbits\n");
  fprintf(asm_out, "\n");

  // Close the output file if we opened one
  if (asm_out) {
    fclose(asm_out);
    asm_out = NULL;
    printf("Assembly generated at file: %s\n", ctds_filename);
    print_generated_assembly_if_debug_flag();
  }
}

void print_generated_assembly_if_debug_flag() {
  if (debug_flag) {
    printf("[DEBUG] Generated Assembly\n");
    FILE *f;
    int c;
    if ((f = fopen(ctds_filename, "r")) == NULL) {
      printf("error in opening a file");
      exit(1);
    }

    while ((c = fgetc(f)) != EOF) {
      printf("%c", c); // printing to the console
    }

    fclose(f);
  }
}