#include "hash.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Print SHA-1("abc"). */
int main(void) {
  uint8_t message[] = {'a', 'b', 'c'};
  uint8_t* digest = sha1(message, sizeof message);

  if (digest == NULL) {
    fprintf(stderr, "sha1: allocation failed\n");
    return 1;
  }

  for (size_t i = 0; i < 20; ++i)
    printf("%.2x", (unsigned int)digest[i]);
  printf("\n");

  free(digest);
  return 0;
}
