#pragma once

#include <stddef.h>

#define PIPELINE_ERROR_MESSAGE_SIZE 512

typedef enum {
  PIPELINE_OK = 0,
  PIPELINE_ERREUR_ARGUMENT = 1,
  PIPELINE_ERREUR_LEXICALE = 2,
  PIPELINE_ERREUR_SYNTAXIQUE = 3,
  PIPELINE_ERREUR_MEMOIRE = 4,
  PIPELINE_ERREUR_EVALUATION = 5
} pipeline_status_t;

int calculer_fx(const char *expression, float x, float *out_resultat,
                char *message_erreur, size_t taille_message_erreur);
