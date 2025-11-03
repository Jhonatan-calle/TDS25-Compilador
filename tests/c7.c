#include <stdbool.h>

int whiletrue = 1;
void main()
{
  bool ifelse = true;
  if (ifelse)
  {
    return;
  }
  else
  {
    int x = 2;
    int y = 3;

    int elsethen = x + y;
    return;
  }
}
int main2()
{
  /*
  comentario multilinea:
  primero declaro las variables
  luego declaro la lista de sentencias
  */

  // declaraciones de variables
  bool bv = true;

  // lista de sentencias
  main();
  whiletrue = 2;
  while (!bv) {
    if (bv) {
      whiletrue = whiletrue + 1;
      bv = !bv;
    } else {
      whiletrue = whiletrue + 2;
    }
  }
  return whiletrue;
}
