#include "../headers/three_address_code.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global TAC list
TACList* tac_list = NULL;

// Function to initialize the TAC list
void init_tac_list() {
    tac_list = (TACList*)malloc(sizeof(TACList));
    if (tac_list == NULL) {
        fprintf(stderr, "Failed to allocate memory for TAC list\n");
        exit(EXIT_FAILURE);
    }
    tac_list->head = NULL;
    tac_list->tail = NULL;
    tac_list->count = 0;
}

// Function to insert a new TAC instruction into the list
void insert_tac(OpCode op, char* result, char* op1, char* op2) {
    if (tac_list == NULL) {
        init_tac_list();
    }

    TAC* new_tac = (TAC*)malloc(sizeof(TAC));
    if (new_tac == NULL) {
        fprintf(stderr, "Failed to allocate memory for new TAC instruction\n");
        exit(EXIT_FAILURE);
    }

    new_tac->op = op;
    new_tac->result = result ? strdup(result) : NULL;
    new_tac->op1 = op1 ? strdup(op1) : NULL;
    new_tac->op2 = op2 ? strdup(op2) : NULL;
    new_tac->next = NULL;

    if (tac_list->head == NULL) {
        tac_list->head = new_tac;
        tac_list->tail = new_tac;
    } else {
        tac_list->tail->next = new_tac;
        tac_list->tail = new_tac;
    }
    tac_list->count++;
}

// Function to print the list of TAC instructions
void print_tac_list() {
    if (tac_list == NULL || tac_list->head == NULL) {
        printf("TAC list is empty.\n");
        return;
    }

    printf("--- Three-Address Code ---\n");
    TAC* current = tac_list->head;
    int i = 0;
    while (current != NULL) {
        printf("%d: ", i++);
        const char* op_str = tac_op_to_string(current->op);

        if (current->op == TAC_LABEL) {
            printf("%s %s\n", op_str, current->result);
        } else if (current->op == TAC_GOTO) {
            printf("%s %s\n", op_str, current->result);
        } else if (current->op == TAC_IFZ) {
            printf("%s %s, GOTO %s\n", op_str, current->op1, current->result);
        } else if (current->op == TAC_ASSIGN) {
            printf("%s := %s\n", current->result, current->op1);
        } else if (current->op == TAC_PARAM || current->op == TAC_RETURN || current->op == TAC_PRINT) {
            printf("%s %s\n", op_str, current->op1);
        } else if (current->op == TAC_CALL) {
            printf("%s := CALL %s\n", current->result, current->op1);
        }
        else {
            printf("%s := %s %s %s\n", current->result, current->op1, op_str, current->op2);
        }
        current = current->next;
    }
    printf("--------------------------\n");
}


// Helper to get string for op
const char* tac_op_to_string(OpCode op) {
    switch (op) {
        case TAC_ADD: return "+";
        case TAC_SUB: return "-";
        case TAC_MUL: return "*";
        case TAC_DIV: return "/";
        case TAC_ASSIGN: return "ASSIGN";
        case TAC_LABEL: return "LABEL";
        case TAC_GOTO: return "GOTO";
        case TAC_IFZ: return "IFZ";
        case TAC_PARAM: return "PARAM";
        case TAC_CALL: return "CALL";
        case TAC_RETURN: return "RETURN";
        case TAC_PRINT: return "PRINT";
        default: return "UNKNOWN";
    }
}
