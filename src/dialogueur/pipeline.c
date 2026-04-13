#include "pipeline.h"

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

static const char *message_erreur_eval(eval_error_t erreur) {
  switch (erreur) {
  case EVAL_OK:
    return "";
  case EVAL_ERREUR_ARBRE_NULL:
    return "arbre d'evaluation nul";
  case EVAL_ERREUR_DIVISION_PAR_ZERO:
    return "division par zero";
  case EVAL_ERREUR_LOG_NON_POSITIF:
    return "logarithme sur une valeur <= 0";
  case EVAL_ERREUR_SQRT_NEGATIF:
    return "racine carree d'une valeur negative";
  case EVAL_ERREUR_NOEUD_INVALIDE:
    return "noeud d'arbre invalide";
  default:
    return "erreur d'evaluation inconnue";
  }
}

void liberer_arbre_pipeline(Arbre arbre) {
  if (arbre == NULL) {
    return;
  }

  liberer_arbre_pipeline(arbre->pjeton_preced);
  liberer_arbre_pipeline(arbre->pjeton_suiv);
  free(arbre);
}

static int
lexical_vector_vers_tableau_entree(const lexical_tokens_vector_t *tokens,
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

int pipeline_build_arbre(const char *expression, Arbre *out_arbre,
                         lexical_error_t *lerr, int *syntax_err) {
  lexical_error_t erreur_lexicale = {0};
  lexical_tokens_vector_t tokens;
  typejeton *entree = NULL;
  typejeton *postfixe = NULL;
  size_t entree_taille = 0;
  Arbre arbre = NULL;
  int status_syntaxe;

  if (out_arbre == NULL) {
    return PIPELINE_ERREUR_ARGUMENT;
  }

  *out_arbre = NULL;
  if (syntax_err != NULL) {
    *syntax_err = 0;
  }

  if (expression == NULL) {
    if (lerr != NULL) {
      lerr->type = NULL_EXPRESSION;
      lerr->at_index = 0;
      snprintf(lerr->message, ERROR_MESSAGE_SIZE, "expression was null");
    }
    return PIPELINE_ERREUR_ARGUMENT;
  }

  tokens = lexical_parse_tokens(expression, &erreur_lexicale);
  if (tokens.size == 0) {
    if (lerr != NULL) {
      *lerr = erreur_lexicale;
    }
    return PIPELINE_ERREUR_LEXICALE;
  }

  if (!lexical_vector_vers_tableau_entree(&tokens, &entree, &entree_taille)) {
    lexical_tokens_vector_free(&tokens);
    return PIPELINE_ERREUR_MEMOIRE;
  }

  postfixe = (typejeton *)malloc(entree_taille * sizeof(typejeton));
  if (postfixe == NULL) {
    free(entree);
    lexical_tokens_vector_free(&tokens);
    return PIPELINE_ERREUR_MEMOIRE;
  }

  status_syntaxe = convertir_en_postfixe(entree, postfixe);
  if (status_syntaxe != SYNTAXE_OK) {
    if (syntax_err != NULL) {
      *syntax_err = status_syntaxe;
    }
    free(postfixe);
    free(entree);
    lexical_tokens_vector_free(&tokens);
    return PIPELINE_ERREUR_SYNTAXIQUE;
  }

  status_syntaxe = convertir_code_postfixe_en_arbre(postfixe, &arbre);
  if (status_syntaxe != SYNTAXE_OK || arbre == NULL) {
    if (syntax_err != NULL) {
      *syntax_err = status_syntaxe;
    }
    free(postfixe);
    free(entree);
    lexical_tokens_vector_free(&tokens);
    return PIPELINE_ERREUR_SYNTAXIQUE;
  }

  *out_arbre = arbre;
  free(postfixe);
  free(entree);
  lexical_tokens_vector_free(&tokens);
  return PIPELINE_OK;
}

int calculer_fx(const char *expression, float x, float *out_resultat,
                char *message_erreur, size_t taille_message_erreur) {
  Arbre arbre = NULL;
  lexical_error_t erreur_lexicale = {0};
  int status_syntaxe = 0;
  int statut_pipeline;

  if (expression == NULL || out_resultat == NULL) {
    copier_message(message_erreur, taille_message_erreur,
                   "arguments invalides pour calculer_fx");
    return PIPELINE_ERREUR_ARGUMENT;
  }

  statut_pipeline = pipeline_build_arbre(expression, &arbre, &erreur_lexicale,
                                         &status_syntaxe);
  if (statut_pipeline != PIPELINE_OK) {
    if (statut_pipeline == PIPELINE_ERREUR_LEXICALE) {
      copier_message(message_erreur, taille_message_erreur,
                     erreur_lexicale.message);
    } else if (statut_pipeline == PIPELINE_ERREUR_SYNTAXIQUE) {
      copier_message(message_erreur, taille_message_erreur,
                     "erreur syntaxique pendant la construction de l'arbre");
    } else if (statut_pipeline == PIPELINE_ERREUR_MEMOIRE) {
      copier_message(
          message_erreur, taille_message_erreur,
          "allocation impossible pendant la construction de l'arbre");
    }
    return statut_pipeline;
  }

  Eval_reset_error();
  *out_resultat = Eval(arbre, x);
  if (Eval_get_error() != EVAL_OK) {
    copier_message(message_erreur, taille_message_erreur,
                   message_erreur_eval(Eval_get_error()));
    liberer_arbre_pipeline(arbre);
    return PIPELINE_ERREUR_EVALUATION;
  }

  liberer_arbre_pipeline(arbre);
  copier_message(message_erreur, taille_message_erreur, "");
  return PIPELINE_OK;
}
