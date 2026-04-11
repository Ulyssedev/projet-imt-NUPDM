#include "pipeline.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  const char *expression;
  float x = 0.0f;
  float resultat = 0.0f;
  char erreur[PIPELINE_ERROR_MESSAGE_SIZE];
  int status;

  if (argc < 2) {
    printf("Usage: %s \"expression\" [x]\n", argv[0]);
    return 1;
  }

  expression = argv[1];
  if (argc >= 3) {
    x = strtof(argv[2], NULL);
  }

  status = calculer_fx(expression, x, &resultat, erreur, sizeof(erreur));
  if (status != PIPELINE_OK) {
    printf("Erreur pipeline (%d): %s\n", status, erreur);
    return 1;
  }

  printf("f(%f) = %f\n", x, resultat);
  return 0;
}
