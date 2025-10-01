#include "../headers/utils.h"

extern char *yytext;

/**
 * Print util function
 *
 * Default print if global debug flag is enabled
 */
void print_if_debug_flag(char *str) {
  if (debug_flag)
    printf("%s\n", str);
}

/**
 * Print util function
 *
 * Calls gen_assembly_code function if global assembly flag is enabled
 */
void gen_assembly_if_assembly_flag(AST *root) {
  if (assembly_flag)
    gen_assembly_code(root);
}

/**
 * Generate the intermediate code
 *
 * Calls gen_inter_code function if global inter code flag is enabled
 */
void gen_inter_code_if_inter_code_flag(AST *root) {
  if (inter_code_flag)
    gen_inter_code(root);
}

/**
 * Utility function to process arguments
 *
 * Receive variables from main and processes them
 * Returns -1 if error, otherwise returns 0
 */
int process_arguments(int argc, char *argv[], char **outfile, char **target,
                      char **opt, char **inputfile) {
  static char outfile_with_ext[256]; // Static to ensure it persists after
                                     // function returns
  *target = NULL;
  *opt = NULL;
  *outfile = "a.out";
  *inputfile = NULL;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "-output") == 0) {
      if (i + 1 < argc) {
        snprintf(outfile_with_ext, sizeof(outfile_with_ext), "%s.out",
                 argv[i + 1]);
        *outfile = outfile_with_ext;
        i++;
      } else {
        fprintf(stderr, "Error: missing argument after -o\n");
        return -1;
      }
    } else if (strcmp(argv[i], "-target") == 0 || strcmp(argv[i], "-t") == 0) {
      if (i + 1 < argc) {
        *target = argv[++i];
      } else {
        fprintf(stderr, "Error: missing argument after -target\n");
        return -1;
      }
    } else if (strcmp(argv[i], "-opt") == 0) {
      if (i + 1 < argc && argv[i + 1][0] != '-') {
        *opt = argv[++i];
      } else {
        *opt = "all";
      }
    } else if (strcmp(argv[i], "-debug") == 0 || strcmp(argv[i], "-d") == 0) {
      debug_flag = 1;
    } else {
      *inputfile = argv[i];
    }
  }

  if (!*inputfile) {
    return -1;
  }

  return 0;
}

/**
 * Parse method
 *
 * Used in compiler main point.
 * yyparse() is called and print if the compilation finished with errors or
 * successfuly.
 */
void parse_method() {
  int compiled_with_errors = yyparse();

  if (compiled_with_errors)
    printf("Compilation completed with errors.\n");
  else
    printf("Compilation completed successfuly.\n");
}

/**
 * Process a given target stage function
 *
 * Used in compiler main point.
 * Given a string target, do the task of a stage conditionally.
 *
 * Retuns 0 if the target is valid.
 * Returns -1 otherwise.
 */
int process_target_stage(const char *target) {
  if (strcmp(target, "scan") == 0) {
    // Lexical analysis
    printf("Stage: Scan\n");
    int token;
    while ((token = yylex()) != 0) {
      switch (token) {
      case V_NUM:
        printf("TOKEN V_NUM: '%s'\n", yytext);
        break;
      case V_TRUE:
        printf("TOKEN V_TRUE: '%s'\n", yytext);
        break;
      case V_FALSE:
        printf("TOKEN V_FALSE: '%s'\n", yytext);
        break;
      case R_EXTERN:
        printf("TOKEN R_EXTERN: '%s'\n", yytext);
        break;
      case R_BOOL:
        printf("TOKEN R_BOOL: '%s'\n", yytext);
        break;
      case R_INTEGER:
        printf("TOKEN R_INTEGER: '%s'\n", yytext);
        break;
      case R_VOID:
        printf("TOKEN R_VOID: '%s'\n", yytext);
        break;
      case R_RETURN:
        printf("TOKEN R_RETURN: '%s'\n", yytext);
        break;
      case R_WHILE:
        printf("TOKEN R_WHILE: '%s'\n", yytext);
        break;
      case R_PROGRAM:
        printf("TOKEN R_PROGRAM: '%s'\n", yytext);
        break;
      case R_IF:
        printf("TOKEN R_IF: '%s'\n", yytext);
        break;
      case R_THEN:
        printf("TOKEN R_THEN: '%s'\n", yytext);
        break;
      case OP_EQ:
        printf("TOKEN OP_EQ: '%s'\n", yytext);
        break;
      case OP_AND:
        printf("TOKEN OP_AND: '%s'\n", yytext);
        break;
      case OP_OR:
        printf("TOKEN OP_OR: '%s'\n", yytext);
        break;
      case ID:
        printf("TOKEN ID: '%s'\n", yytext);
        break;
      default:
        printf("operatorDelimiter %d: '%s'\n", token, yytext);
      }
    }
    printf("EOF\n");
    return 0;
  } else if (strcmp(target, "parse") == 0) {
    // Syntax analysis
    printf("Stage: Parse\n");
    parse_method();
    return 0;
  } else if (strcmp(target, "codinter") == 0) {
    // Intermedium code
    printf("Stage: Intermedium code\n");
    inter_code_flag = 1;
    return 0;
  } else if (strcmp(target, "assembly") == 0) {
    // Assembly
    printf("Stage: Assembly\n");
    assembly_flag = 1;
    parse_method();
    return 0;
  }

  // Invalid target stage
  return -1;
}

/**
 * Utility function called when a flag is not valid.
 *
 * Used in compiler main point.
 * Prints in stderr a message to help the user how to use the executable.
 * Exits with EXIT_FAILURE after prints.
 */
void usage_message(const char *prog) {
  fprintf(stderr, "Usage: %s [flags] file.ctds\n", prog);
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -o <output>        Renames the executable\n");
  fprintf(stderr,
          "  -target <stage>    Stage: scan | parse | codinter | assembly\n");
  fprintf(stderr, "  -opt [opt]         Optimizations (all, ...)\n");
  fprintf(stderr, "  -debug             Debug info\n");
  exit(EXIT_FAILURE);
}

/**
 * Creates the output file as a symbolic link to the compiled binary.
 *
 * Given the outfile string name for the output file, creates a symbolic link
 * with the c-tds object main executable.
 * Return 0 on success
 * Return -1 on error
 */
int create_output_file(const char *outfile) {
  if (!outfile)
    return 0; // No output file specified, nothing to do

  // Remove existing file if it exists
  if (access(outfile, F_OK) == 0) {
    if (unlink(outfile) != 0) {
      perror("Error removing existing output file");
      return -1;
    }
  }

  // Create symbolic link with specified name
  if (symlink("c-tds", outfile) != 0) {
    perror("Error creating output file");
    return -1;
  }

  if (debug_flag) {
    printf("[DEBUG] Created output file: %s\n", outfile);
  }

  return 0;
}

/**
 * "Tipos" Enum type to String
 *
 * Returns a string corresponding to the enum received.
 */
const char *tipoDatoToStr(Tipos type) {
  switch (type) {
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

/**
 * "TipoNodo" Enum type to String
 *
 * Retuns a string corresponding to the enum received.
 */
const char *tipoNodoToStr(TipoNodo t) {
  switch (t) {
  case TR_PROGRAM:
    return "TR_PROGRAM";
  case TR_METHOD_DECLARATION:
    return "TR_METHOD_DECLARATION";
  case TR_VAR_DECLARATION:
    return "TR_VAR_DECLARATION";
  case TR_DECLARATION_LIST:
    return "TR_DECLARATION_LIST";
  case TR_EXTERN:
    return "TR_EXTERN";
  case TR_PARAM_LIST:
    return "TR_PARAM_LIST";
  case TR_PARAM:
    return "TR_PARAM";
  case TR_BLOCK:
    return "TR_BLOCK";
  case TR_SENTENCES_LIST:
    return "TR_SENTENCES_LIST";
  case TR_ASSIGN:
    return "TR_ASSIGN";
  case TR_INVOCATION:
    return "TR_INVOCATION";
  case TR_IF_STATEMENT:
    return "TR_IF_STATEMENT";
  case TR_ELSE_BODY:
    return "TR_ELSE_BODY";
  case TR_WHILE_STATEMENT:
    return "TR_WHILE_STATEMENT";
  case TR_RETURN:
    return "TR_RETURN";
  case TR_ARG_LIST:
    return "TR_ARG_LIST";
  case TR_IDENTIFIER:
    return "TR_IDENTIFIER";
  case TR_LOGIC_NEGATION:
    return "TR_LOGIC_NEGATION";
  case TR_ARITHMETIC_NEGATION:
    return "TR_ARITHMETIC_NEGATION";
  case TR_ADDITION:
    return "TR_ADDITION";
  case TR_SUBSTRACTION:
    return "TR_SUBSTRACTION";
  case TR_MULTIPLICATION:
    return "TR_MULTIPLICATION";
  case TR_DIVITION:
    return "TR_DIVITION";
  case TR_MODULO:
    return "TR_MODULO";
  case TR_LESS_THAN:
    return "TR_LESS_THAN";
  case TR_GREATER_THAN:
    return "TR_GREATER_THAN";
  case TR_LOGIC_EQUAL:
    return "TR_LOGIC_EQUAL";
  case TR_AND:
    return "TR_AND";
  case TR_OR:
    return "TR_OR";
  case TR_VALUE:
    return "TR_VALUE";
  default:
    return "UNKNOWN";
  }
}
