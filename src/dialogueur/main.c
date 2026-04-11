#include "pipeline.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIALOGUEUR_BUFFER_SIZE 512

static void supprimer_retour_ligne(char *s) {
  size_t n;

  if (s == NULL) {
    return;
  }

  n = strlen(s);
  if (n > 0 && s[n - 1] == '\n') {
    s[n - 1] = '\0';
  }
}

static int ligne_vide(const char *s) {
  size_t i;

  if (s == NULL) {
    return 1;
  }

  for (i = 0; s[i] != '\0'; i++) {
    if (!isspace((unsigned char)s[i])) {
      return 0;
    }
  }
  return 1;
}

static int lire_flottant(const char *texte, float *valeur) {
  char *fin;
  float tmp;

  if (texte == NULL || valeur == NULL) {
    return 0;
  }

  tmp = strtof(texte, &fin);
  if (fin == texte) {
    return 0;
  }

  while (*fin != '\0') {
    if (!isspace((unsigned char)*fin)) {
      return 0;
    }
    fin++;
  }

  *valeur = tmp;
  return 1;
}

int main(int argc, char **argv) {
  char expression[DIALOGUEUR_BUFFER_SIZE];
  char saisie_min[DIALOGUEUR_BUFFER_SIZE];
  char saisie_max[DIALOGUEUR_BUFFER_SIZE];
  char saisie_choix[DIALOGUEUR_BUFFER_SIZE];
  float borne_min;
  float borne_max;
  float point_test;
  float resultat = 0.0f;
  char erreur[PIPELINE_ERROR_MESSAGE_SIZE];
  int status;
  int en_mode_ligne_commande = (argc >= 2);

  if (en_mode_ligne_commande) {
    float x = 0.0f;

    if (argc >= 3) {
      x = strtof(argv[2], NULL);
    }

    status = calculer_fx(argv[1], x, &resultat, erreur, sizeof(erreur));
    if (status != PIPELINE_OK) {
      printf("Erreur pipeline (%d): %s\n", status, erreur);
      return 1;
    }

    printf("f(%f) = %f\n", x, resultat);
    return 0;
  }

  for (;;) {
    printf("\nExpression (ou 'q' pour quitter): ");
    if (fgets(expression, sizeof(expression), stdin) == NULL) {
      return 0;
    }
    supprimer_retour_ligne(expression);

    if (strcmp(expression, "q") == 0 || strcmp(expression, "quit") == 0) {
      break;
    }

    if (ligne_vide(expression)) {
      printf("Expression vide, recommencez.\n");
      continue;
    }

    for (;;) {
      printf("Borne inferieure: ");
      if (fgets(saisie_min, sizeof(saisie_min), stdin) == NULL) {
        return 0;
      }
      supprimer_retour_ligne(saisie_min);
      if (!lire_flottant(saisie_min, &borne_min)) {
        printf("Borne inferieure invalide.\n");
        continue;
      }

      printf("Borne superieure: ");
      if (fgets(saisie_max, sizeof(saisie_max), stdin) == NULL) {
        return 0;
      }
      supprimer_retour_ligne(saisie_max);
      if (!lire_flottant(saisie_max, &borne_max)) {
        printf("Borne superieure invalide.\n");
        continue;
      }

      if (borne_min >= borne_max) {
        printf("Intervalle invalide: borne inferieure doit etre < borne superieure.\n");
        continue;
      }

      break;
    }

    point_test = (borne_min + borne_max) / 2.0f;
    status = calculer_fx(expression, point_test, &resultat, erreur, sizeof(erreur));
    if (status != PIPELINE_OK) {
      printf("Erreur pipeline (%d): %s\n", status, erreur);
      continue;
    }

    printf("Expression validee sur [%.3f ; %.3f], test au milieu x=%.3f -> f(x)=%.6f\n",
           borne_min, borne_max, point_test, resultat);

    printf("Nouvelle saisie ? [Entree/o = oui, n = non] : ");
    if (fgets(saisie_choix, sizeof(saisie_choix), stdin) == NULL) {
      return 0;
    }
    supprimer_retour_ligne(saisie_choix);
    if (saisie_choix[0] == 'n' || saisie_choix[0] == 'N') {
      break;
    }
  }

  return 0;
}
