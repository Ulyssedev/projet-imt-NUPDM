#pragma once

#include <stddef.h>
#include "../evaluateur/eval.h"
#include "../lexical/lexical.h"
#include "../syntaxique/syntaxique.h"

#include <stdio.h>
#include <stdlib.h>

#define PIPELINE_ERROR_MESSAGE_SIZE 512

typedef enum {
  PIPELINE_OK = 0,
  PIPELINE_ERREUR_ARGUMENT = 1,
  PIPELINE_ERREUR_LEXICALE = 2,
  PIPELINE_ERREUR_SYNTAXIQUE = 3,
  PIPELINE_ERREUR_MEMOIRE = 4,
  PIPELINE_ERREUR_EVALUATION = 5
} pipeline_status_t;

#define PIPELINE_MEM_ERROR PIPELINE_ERREUR_MEMOIRE
#define PIPELINE_LEXICAL_ERROR PIPELINE_ERREUR_LEXICALE
#define PIPELINE_SYNTAX_ERROR PIPELINE_ERREUR_SYNTAXIQUE

void liberer_arbre_pipeline(Arbre arbre);

int pipeline_build_arbre(const char *expression, Arbre *out_arbre,
                         lexical_error_t *lerr, int *syntax_err);

int calculer_fx(const char *expression, float x, float *out_resultat,
                char *message_erreur, size_t taille_message_erreur);
