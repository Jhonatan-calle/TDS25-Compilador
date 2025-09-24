#include "../headers/main.h"
#include <stdlib.h>
#include <unistd.h>

extern char *yytext;

Scope *scope = NULL;
int debug_flag = 0;
int assembly_flag = 0;

int compiler_main(int argc, char *argv[]) {
  scope = malloc(sizeof(Scope));
  scope->tail = NULL;
  inicialize_scope();

  char outfile_with_ext[256]; // Buffer for filename with extension
  char *outfile = "a.out";    // Default output file name
  char *target = NULL;
  char *opt = NULL;

  char *inputfile = NULL;

  // Cycle the arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "-output") == 0) {
      if (i + 1 < argc) {
        snprintf(outfile_with_ext, sizeof(outfile_with_ext), "%s.out",
                 argv[i + 1]);
        outfile = outfile_with_ext;
        i++;
      } else {
        fprintf(stderr, "Error: missing argument after -o\n");
        usage_message(argv[0]);
      }
    } else if (strcmp(argv[i], "-target") == 0 || strcmp(argv[i], "-t") == 0) {
      if (i + 1 < argc) {
        target = argv[++i];
      } else {
        fprintf(stderr, "Error: missing argument after -target\n");
        usage_message(argv[0]);
      }
    } else if (strcmp(argv[i], "-opt") == 0) {
      if (i + 1 < argc && argv[i + 1][0] != '-') {
        opt = argv[++i]; // It can be 'all' or another
      } else {
        opt = "all"; // Assumming 'all' if nothing provided
      }
    } else if (strcmp(argv[i], "-debug") == 0 || strcmp(argv[i], "-d") == 0) {
      debug_flag = 1;
    } else {
      // If its not an option, assumming its the input file
      inputfile = argv[i];
    }
  }

  if (!inputfile) {
    usage_message(argv[0]);
    return EXIT_FAILURE;
  }

  // Open source file
  yyin = fopen(inputfile, "r");
  if (!yyin) {
    perror("Error opening input file");
    usage_message(argv[0]);
  }

  // Debug info
  if (debug_flag) {
    printf("[DEBUG] Input file: %s\n", inputfile);
    if (outfile)
      printf("[DEBUG] Out file: %s\n", outfile);
    if (target)
      printf("[DEBUG] Target: %s\n", target);
    if (opt)
      printf("[DEBUG] Optimizations: %s\n", opt);
  }

  if (target && strcmp(target, "scan") == 0) {
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
  } else if (target && strcmp(target, "parse") == 0) {
    // Syntax analysis
    printf("Stage: Parse\n");
    parse_method();
  } else if (target && strcmp(target, "codinter") == 0) {
    // Intermedium code
    printf("Stage: Intermedium code\n");
  } else if (target && strcmp(target, "assembly") == 0) {
    // Assembly
    printf("Stage: Assembly\n");
    assembly_flag = 1;
    parse_method();
  } else if (target) {
    // Target flag set but invalid stage provided
    usage_message(argv[0]);
  } else {
    // Normal compilation
    parse_method();
  }

  fclose(yyin);

  // Create symbolic link with specified output name
  if (outfile) {
    // Remove existing file if it exists
    if (access(outfile, F_OK) == 0) {
      unlink(outfile);
    }
    // Create symbolic link with specified name
    if (symlink("c-tds", outfile) != 0) {
      perror("Error creating output file");
      return EXIT_FAILURE;
    }
    if (debug_flag) {
      printf("[DEBUG] Created output file: %s\n", outfile);
    }
  }

  return 0;
}

void parse_method() {
  int compiled_with_errors = yyparse();

  if (compiled_with_errors)
    printf("Compilation completed with errors.\n");
  else
    printf("Compilation completed successfuly.\n");
}

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
