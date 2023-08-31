#include <stdio.h>
#include <stdlib.h>

int
main (int argc, char **argv)
{
  int *a = malloc (sizeof (int) * 10);
  a[5]   = 0;
  if (a[argc]) // access uninitialized memory
    printf ("xx\n");
  (void) argc;
  (void) argv;
  free (a);
  return 0;
}
