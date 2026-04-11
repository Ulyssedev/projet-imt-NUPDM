#include "pipeline.h"

#include "../evaluateur/eval.h"
#include "../lexical/lexical.h"
#include "../syntaxique/syntaxique.h"

#include <stdio.h>
#include <stdlib.h>

static void copier_message(char *destination, size_t taille_destination,
                           const char *message) {
  if (destination == NULL || taille_destination == 0) {
    return;
  }

  if (message == NULL) {
    destination[0] = '\0';
    return;
  }

  snprintf(destination, taille_destination, "%s", message);
}

static void liberer_arbre_pipeline(Arbre arbre) {
  if (arbre == NULL) {
    return;
  }

  liberer_arbre_pipeline(arbre->pjeton_preced);
  liberer_arbre_pipeline(arbre->pjeton_suiv);
  free(arbre);
}

static int lexical_vector_vers_tableau_entree(const lexical_tokens_vector_t *tokens,
                                              typejeton **sortie_tableau,
                                              size_t *sortie_taille) {
  typejeton *tableau;
  size_t i;

  if (tokens == NULL || sortie_tableau == NULL || sortie_taille == NULL) {
    return 0;
  }

  *sortie_tableau = NULL;
  *sortie_taille = 0;

  tableau = (typejeton *)malloc((tokens->size + 1) * sizeof(typejeton));
  if (tableau == NULL) {
    return 0;
  }

  for (i = 0; i < tokens->size; i++) {
    tableau[i] = tokens->tokens[i];
  }

  tableau[tokens->size].lexem = FIN;
  tableau[tokens->size].valeur.reel = 0.0f;

  *sortie_tableau = tableau;
  *sortie_taille = tokens->size + 1;
  return 1;
}

int calculer_fx(const char *expression, float x, float *out_resultat,
                char *message_erreur, size_t taille_message_erreur) {
  lexical_error_t erreur_lexicale = {0};
  lexical_tokens_vector_t tokens;
  typejeton *entree = NULL;
  typejeton *postfixe = NULL;
  size_t entree_taille = 0;
  Arbre arbre = NULL;
  int status_syntaxe;

  if (expression == NULL || out_resultat == NULL) {
    copier_message(message_erreur, taille_message_erreur,
                   "arguments invalides pour calculer_fx");
    return PIPELINE_ERREUR_ARGUMENT;
  }

  tokens = lexical_parse_tokens(expression, &erreur_lexicale);
  if (tokens.size == 0) {
    copier_message(message_erreur, taille_message_erreur, erreur_lexicale.message);
    return PIPELINE_ERREUR_LEXICALE;
  }

  if (!lexical_vector_vers_tableau_entree(&tokens, &entree, &entree_taille)) {
    lexical_tokens_vector_free(&tokens);
    copier_message(message_erreur, taille_message_erreur,
                   "allocation impossible pour les tokens d'entree");
    return PIPELINE_ERREUR_MEMOIRE;
  }

  postfixe = (typejeton *)malloc(entree_taille * sizeof(typejeton));
  if (postfixe == NULL) {
    free(entree);
    lexical_tokens_vector_free(&tokens);
    copier_message(message_erreur, taille_message_erreur,
                   "allocation impossible pour le postfixe");
    return PIPELINE_ERREUR_MEMOIRE;
  }

  status_syntaxe = convertir_en_postfixe(entree, postfixe);
  if (status_syntaxe != SYNTAXE_OK) {
    free(postfixe);
    free(entree);
    lexical_tokens_vector_free(&tokens);
    copier_message(message_erreur, taille_message_erreur,
                   "erreur syntaxique pendant la conversion postfixe");
    return PIPELINE_ERREUR_SYNTAXIQUE;
  }

  status_syntaxe = convertir_code_postfixe_en_arbre(postfixe, &arbre);
  if (status_syntaxe != SYNTAXE_OK || arbre == NULL) {
    free(postfixe);
    free(entree);
    lexical_tokens_vector_free(&tokens);
    copier_message(message_erreur, taille_message_erreur,
                   "erreur syntaxique pendant la conversion arbre");
    return PIPELINE_ERREUR_SYNTAXIQUE;
  }

  *out_resultat = Eval(arbre, x);

  liberer_arbre_pipeline(arbre);
  free(postfixe);
  free(entree);
  lexical_tokens_vector_free(&tokens);
  copier_message(message_erreur, taille_message_erreur, "");
  return PIPELINE_OK;
}
