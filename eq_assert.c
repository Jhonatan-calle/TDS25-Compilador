#include <stdio.h>

#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED   "\033[0;31m"
#define COLOR_RESET "\033[0m"

int eq_assert(int expected, int result) {
  if (expected == result) {
    printf(COLOR_GREEN "Test pass\n" COLOR_RESET);
    return 0;
  } else {
    printf(COLOR_RED "Test failed\n" COLOR_RESET);
    return 1;
  }
}
