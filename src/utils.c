#include "../headers/utils.h"
#include "../headers/main.h"

void print_if_debug(char* str)
{
  if (debug)
    printf("%s\n", str);
}
