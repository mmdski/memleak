#include <stdlib.h>
int *p;
int
main (void)
{
  p = malloc (7);
  free (p);
  *p = 10;
  return 0;
}
