#include "../src/syntaxique/syntaxique.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct {
  const char *nom;
  typejeton *entree;
  int taille_entree;
  int retour_attendu;
  typejeton *sortie_attendue;
  int taille_sortie_attendue;
} TestCase;

static int jetons_egaux(typejeton a, typejeton b) {
  if (a.lexem != b.lexem) {
    return 0;
  }

  switch (a.lexem) {
  case REEL:
    return a.valeur.reel == b.valeur.reel;
  case OPERATEUR:
    return a.valeur.operateur == b.valeur.operateur;
  case FONCTION:
    return a.valeur.fonction == b.valeur.fonction;
  default:
    return 1;
  }
}

static int verifier_sortie(typejeton sortie[], typejeton attendu[], int taille) {
  int i;

  for (i = 0; i < taille; i++) {
    if (!jetons_egaux(sortie[i], attendu[i])) {
      return 0;
    }
  }

  return 1;
}

static int executer_test(const TestCase *test) {
  typejeton sortie[128];
  int retour;

  (void)test->taille_entree;

  retour = convertir_en_postfixe(test->entree, sortie);
  if (retour != test->retour_attendu) {
    printf("[ECHEC] %s: retour=%d attendu=%d\n", test->nom, retour,
           test->retour_attendu);
    return 0;
  }

  if (test->retour_attendu == SYNTAXE_OK &&
      !verifier_sortie(sortie, test->sortie_attendue,
                       test->taille_sortie_attendue)) {
    printf("[ECHEC] %s: sortie postfixee incorrecte\n", test->nom);
    return 0;
  }

  printf("[OK] %s\n", test->nom);
  return 1;
}

static void liberer_arbre_test(Arbre arbre) {
  if (arbre == NULL) {
    return;
  }

  liberer_arbre_test(arbre->pjeton_preced);
  liberer_arbre_test(arbre->pjeton_suiv);
  free(arbre);
}

static int verifier_arbre_test(Arbre arbre) {
  if (arbre == NULL) {
    return 0;
  }

  if (arbre->jeton.lexem != OPERATEUR || arbre->jeton.valeur.operateur != PLUS) {
    return 0;
  }

  if (arbre->pjeton_preced == NULL || arbre->pjeton_suiv == NULL) {
    return 0;
  }

  if (arbre->pjeton_preced->jeton.lexem != OPERATEUR ||
      arbre->pjeton_preced->jeton.valeur.operateur != FOIS) {
    return 0;
  }

  if (arbre->pjeton_preced->pjeton_preced == NULL ||
      arbre->pjeton_preced->pjeton_suiv == NULL) {
    return 0;
  }

  if (arbre->pjeton_preced->pjeton_preced->jeton.lexem != REEL ||
      arbre->pjeton_preced->pjeton_preced->jeton.valeur.reel != 3.0f) {
    return 0;
  }

  if (arbre->pjeton_preced->pjeton_suiv->jeton.lexem != VARIABLE) {
    return 0;
  }

  if (arbre->pjeton_suiv->jeton.lexem != REEL ||
      arbre->pjeton_suiv->jeton.valeur.reel != 5.0f) {
    return 0;
  }

  return 1;
}

static int executer_test_postfixe_vers_arbre(void) {
  typejeton code_postfixe[] = {{REEL, {.reel = 3.0f}},
                               {VARIABLE, {0}},
                               {OPERATEUR, {.operateur = FOIS}},
                               {REEL, {.reel = 5.0f}},
                               {OPERATEUR, {.operateur = PLUS}},
                               {FIN, {0}}};
  Arbre arbre = NULL;
  int retour = convertir_code_postfixe_en_arbre(code_postfixe, &arbre);

  if (retour != SYNTAXE_OK) {
    printf("[ECHEC] postfixe vers arbre: retour=%d attendu=%d\n", retour,
           SYNTAXE_OK);
    return 0;
  }

  if (!verifier_arbre_test(arbre)) {
    printf("[ECHEC] postfixe vers arbre: structure d'arbre incorrecte\n");
    liberer_arbre_test(arbre);
    return 0;
  }

  liberer_arbre_test(arbre);
  printf("[OK] postfixe vers arbre\n");
  return 1;
}

int main(void) {
  int i;
  int nb_ok = 0;

  typejeton entree_ok[] = {{PAR_OUV, {0}},
                           {REEL, {.reel = 3.14f}},
                           {OPERATEUR, {.operateur = FOIS}},
                           {REEL, {.reel = 2.71f}},
                           {PAR_FERM, {0}},
                           {FIN, {0}}};
  typejeton sortie_ok[] = {{REEL, {.reel = 3.14f}},
                           {REEL, {.reel = 2.71f}},
                           {OPERATEUR, {.operateur = FOIS}}};

  typejeton entree_fonction[] = {
      {FONCTION, {.fonction = SIN}}, {VARIABLE, {0}}, {FIN, {0}}};
  typejeton sortie_fonction[] = {{VARIABLE, {0}},
                                 {FONCTION, {.fonction = SIN}}};

  typejeton entree_sans_fin[] = {{REEL, {.reel = 1.0f}}};

  typejeton entree_invalide[] = {{PAR_OUV, {0}},
                                 {REEL, {.reel = 1.0f}},
                                 {OPERATEUR, {.operateur = PLUS}},
                                 {PAR_FERM, {0}},
                                 {FIN, {0}}};

  TestCase tests[] = {
      {"binaire parenthese", entree_ok, 6, SYNTAXE_OK, sortie_ok, 3},
      {"fonction unaire", entree_fonction, 3, SYNTAXE_OK, sortie_fonction, 2},
      {"fin manquante", entree_sans_fin, 1, SYNTAXE_ERREUR_FIN_MANQUANTE, NULL,
       0},
      {"grammaire invalide", entree_invalide, 5, SYNTAXE_ERREUR_GRAMMAIRE, NULL,
       0},
  };

  int nb_tests = (int)(sizeof(tests) / sizeof(tests[0]));

  for (i = 0; i < nb_tests; i++) {
    if (executer_test(&tests[i])) {
      nb_ok++;
    }
  }

  if (executer_test_postfixe_vers_arbre()) {
    nb_ok++;
  }
  nb_tests++;

  printf("\nResultat: %d/%d tests valides\n", nb_ok, nb_tests);

  return (nb_ok == nb_tests) ? 0 : 1;
}
