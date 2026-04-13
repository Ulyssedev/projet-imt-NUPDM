#include "../src/common/jeton.h"
#include "../src/evaluateur/eval.h"
#include <stdio.h>
#include <stdlib.h>

// Permet de créer un noeud en une seule ligne au lieu de tout réécrire à chaque fois
Node *creer_noeud(typetoken lexem, typevaleur val, Node *gauche, Node *droite) {
  Node *nouveau = (Node *)malloc(sizeof(Node));
  if (nouveau == NULL) {
    printf("Erreur d'allocation memoire\n");
    exit(1);
  }
  nouveau->jeton.lexem = lexem;
  nouveau->jeton.valeur = val;
  nouveau->pjeton_preced = gauche;
  nouveau->pjeton_suiv = droite;
  return nouveau;
}

// Parcours l'arbre pour tout supprimer à la fin et éviter les fuites mémoire
void liberer_arbre(Arbre A) {
  if (A == NULL)
    return;
  liberer_arbre(A->pjeton_preced);
  liberer_arbre(A->pjeton_suiv);
  free(A);
}

// Nos tests

// Test 1 : Vérifie qu'on arrive bien à construire et calculer "3 * x + 5"
void test_expression_simple() {
  printf("\n--- Test 1 : Expression simple 3 * x + 5 ---\n");
  Eval_reset_error(); // On remet les erreurs à zéro pour repartir bien

  // Création des feuilles de notre arbre (les constantes et la variable)
  typevaleur val_3 = {.reel = 3.0};
  Node *noeud_3 = creer_noeud(REEL, val_3, NULL, NULL);

  typevaleur val_5 = {.reel = 5.0};
  Node *noeud_5 = creer_noeud(REEL, val_5, NULL, NULL);

  typevaleur val_vide = {0};
  Node *noeud_x = creer_noeud(VARIABLE, val_vide, NULL, NULL);

  // Création du noeud de multiplication (3 * x)
  typevaleur val_fois = {.operateur = FOIS};
  Node *noeud_mult = creer_noeud(OPERATEUR, val_fois, noeud_3, noeud_x);

  // On assemble le tout avec l'addition finale (la racine de l'arbre)
  typevaleur val_plus = {.operateur = PLUS};
  Node *racine = creer_noeud(OPERATEUR, val_plus, noeud_mult, noeud_5);

  // On teste l'évaluation avec x = 2
  float x_test = 2.0f;
  float resultat = Eval(racine, x_test);

  printf("Equation testee : 3 * x + 5\n");
  printf("Valeur de x     : %.2f\n", x_test);
  printf("Resultat obtenu : %.2f\n", resultat);
  printf("Resultat attendu: 11.00\n");

  // On vérifie que le calcul est bon et qu'il n'y a pas eu d'erreur déclenchée
  // pendant le parcours
  if (resultat == 11.0f && Eval_get_error() == EVAL_OK) {
    printf(">> Test REUSSI \n");
  } else {
    printf(">> Enorme probleme, le test a echoue.\n");
  }

  liberer_arbre(racine);
}

// Test 2 : Vérifie qu'on intercepte bien les divisions par zéro
void test_division_par_zero() {
  printf("\n--- Test 2 : Division par zero ---\n");
  Eval_reset_error();

  // On prépare l'arbre pour faire "10 / x"
  typevaleur v_10 = {.reel = 10.0};
  Node *n_10 = creer_noeud(REEL, v_10, NULL, NULL);

  typevaleur v_vide = {0};
  Node *n_x = creer_noeud(VARIABLE, v_vide, NULL, NULL);

  typevaleur v_div = {.operateur = DIV};
  Node *racine = creer_noeud(OPERATEUR, v_div, n_10, n_x);

  // On lance le calcul avec x = 0 exprès pour forcer l'erreur
  float res = Eval(racine, 0.0f);

  printf("Equation testee : 10 / x (avec x = 0)\n");

  // La fonction Eval aurait dû lever l'erreur spécifique dans la variable
  // globale
  if (Eval_get_error() == EVAL_ERREUR_DIVISION_PAR_ZERO) {
    printf(">> Test REUSSI ! \n");
  } else {
    printf(">> Enorme probleme, pas d'erreur detectee.\n");
  }

  liberer_arbre(racine);
}

// Test 3 : Vérifie la précision de nos fonctions trigo (Taylor)
void test_identite_trigo_extreme() {
  printf("\n--- Test 3 : Identite cos^2(x) + sin^2(x) = 1 ---\n");
  Eval_reset_error();

  // On crée les variables et les constantes pour les exposants
  typevaleur v_vide = {0};
  Node *n_x1 = creer_noeud(VARIABLE, v_vide, NULL, NULL);
  Node *n_x2 = creer_noeud(VARIABLE, v_vide, NULL, NULL);

  typevaleur v_2 = {.reel = 2.0};
  Node *n_2_a = creer_noeud(REEL, v_2, NULL, NULL);
  Node *n_2_b = creer_noeud(REEL, v_2, NULL, NULL);

  // On applique les fonctions cos(x) et sin(x)
  typevaleur v_cos = {.fonction = COS};
  Node *n_cos = creer_noeud(FONCTION, v_cos, n_x1, NULL);

  typevaleur v_sin = {.fonction = SIN};
  Node *n_sin = creer_noeud(FONCTION, v_sin, n_x2, NULL);

  // On met le tout au carré : cos(x)^2 et sin(x)^2
  typevaleur v_puis = {.operateur = PUIS};
  Node *n_cos_carre = creer_noeud(OPERATEUR, v_puis, n_cos, n_2_a);
  Node *n_sin_carre = creer_noeud(OPERATEUR, v_puis, n_sin, n_2_b);

  // On additionne les deux blocs
  typevaleur v_plus = {.operateur = PLUS};
  Node *racine = creer_noeud(OPERATEUR, v_plus, n_cos_carre, n_sin_carre);

  // On teste avec une valeur de x difficile à évaluer pour nos séries de Taylor
  float x_test = 1.5f;
  float res = Eval(racine, x_test);

  printf("Equation testee : cos(x)^2 + sin(x)^2\n");
  printf("Valeur de x     : %.2f\n", x_test);
  printf("Resultat obtenu : %.5f\n", res);

  // Le résultat ne sera jamais *parfaitement* 1.0 à cause de la précision des
  // floats, donc on laisse une petite marge d'erreur logique
  if (res > 0.99f && res < 1.01f) {
    printf(">> Test REUSSI ! \n");
  } else {
    printf(">> Enorme probleme, le resultat n'est pas 1.\n");
  }

  liberer_arbre(racine);
}

// Test 4 : La fonction exponentielle

void test_exponentielle() {
  printf("\n--- Test 4 : Exponentielle exp(x) avec x = 1 ---\n");
  Eval_reset_error();

  // On crée juste la feuille pour la variable x
  typevaleur v_vide = {0};
  Node *n_x = creer_noeud(VARIABLE, v_vide, NULL, NULL);

  // On applique la fonction exponentielle à notre x
  typevaleur v_exp = {.fonction = EXP};
  Node *racine = creer_noeud(FONCTION, v_exp, n_x, NULL);

  // On lance le calcul avec x = 1
  float x_test = 1.0f;
  float res = Eval(racine, x_test);

  printf("Equation testee : exp(x)\n");
  printf("Valeur de x     : %.2f\n", x_test);
  printf("Resultat obtenu : %.5f\n", res);
  printf("Resultat attendu: ~2.71828\n");

  // On vérifie qu'on est bien autour de 2.718 (on laisse une petite marge pour
  // les floats)
  if (res > 2.71f && res < 2.72f && Eval_get_error() == EVAL_OK) {
    printf(">> Test REUSSI ! \n");
  } else {
    printf(">> Enorme probleme, l'approximation est fausse (verifie ta boucle "
           "dans my_exp).\n");
  }

  liberer_arbre(racine);
}

// Test 5 : Les fonctions limites (Tangente et Sinus Cardinal en zéro)
// On teste ici un point critique : x = 0.
void test_limites_zero() {
  printf("\n--- Test 5 : Limites avec sinc(x) + tan(x) pour x = 0 ---\n");
  Eval_reset_error();

  typevaleur v_vide = {0};
  Node *n_x1 = creer_noeud(VARIABLE, v_vide, NULL, NULL);
  Node *n_x2 = creer_noeud(VARIABLE, v_vide, NULL, NULL);

  // sinc(x)
  typevaleur v_sinc = {.fonction = SINC};
  Node *n_sinc = creer_noeud(FONCTION, v_sinc, n_x1, NULL);

  // tan(x)
  typevaleur v_tan = {.fonction = TAN};
  Node *n_tan = creer_noeud(FONCTION, v_tan, n_x2, NULL);

  // sinc(x) + tan(x)
  typevaleur v_plus = {.operateur = PLUS};
  Node *racine = creer_noeud(OPERATEUR, v_plus, n_sinc, n_tan);

  // Lancement du calcul
  float x_test = 0.0f;
  float res = Eval(racine, x_test);

  printf("Equation testee : sinc(x) + tan(x)\n");
  printf("Resultat obtenu : %.2f\n", res);
  // sinc(0) doit renvoyer 1.0 (ta condition spéciale), et tan(0) renvoie 0.0
  printf("Resultat attendu: 1.00\n");

  if (res == 1.0f && Eval_get_error() == EVAL_OK) {
    printf(
        ">> Test REUSSI ! \n");
  } else {
    printf(">> Probleme avec les limites en zero.\n");
  }

  liberer_arbre(racine);
}

// Test 6 : Racine carrée et Logarithme (sur des valeurs connues)
void test_racine_et_log() {
  printf("\n--- Test 6 : sqrt(x) et log(y) ---\n");
  Eval_reset_error();

  // On va utiliser le même noeud x pour tester deux arbres séparément
  typevaleur v_vide = {0};
  Node *n_x = creer_noeud(VARIABLE, v_vide, NULL, NULL);

  // Arbre 1 : sqrt(x)
  typevaleur v_sqrt = {.fonction = SQRT};
  Node *racine_sqrt = creer_noeud(FONCTION, v_sqrt, n_x, NULL);
  float res_sqrt = Eval(racine_sqrt, 9.0f); // On s'attend à 3.0

  // Arbre 2 : log(x)
  typevaleur v_log = {.fonction = LOG};
  Node *racine_log = creer_noeud(FONCTION, v_log, n_x, NULL);
  float res_log = Eval(racine_log, 1.0f); // On s'attend à 0.0

  printf("Test 1 -> sqrt(9) = %.2f (Attendu : 3.00)\n", res_sqrt);
  printf("Test 2 -> log(1)  = %.2f (Attendu : 0.00)\n", res_log);

  // Tolérance pour les floats, surtout pour Héron et Taylor
  if (res_sqrt > 2.99f && res_sqrt < 3.01f && res_log > -0.01f &&
      res_log < 0.01f) {
    printf(">> Test REUSSI ! \n");
  } else {
    printf(">> Oups, verifie tes approximations.\n");
  }

  liberer_arbre(racine_sqrt);
  liberer_arbre(racine_log);
}

// Test 7 : Les utilitaires (valeur absolue, negatif, entier)
// On va imbriquer les trois : entier(abs(val_neg(x)))
void test_utilitaires() {
  printf("\n--- Test 7 : Imbrication ENTIER( ABS( VAL_NEG(x) ) ) ---\n");
  Eval_reset_error();

  typevaleur v_vide = {0};
  Node *n_x = creer_noeud(VARIABLE, v_vide, NULL, NULL);

  // 1. val_neg(x)
  typevaleur v_neg = {.fonction = VAL_NEG};
  Node *n_neg = creer_noeud(FONCTION, v_neg, n_x, NULL);

  // 2. abs(...)
  typevaleur v_abs = {.fonction = ABS};
  Node *n_abs = creer_noeud(FONCTION, v_abs, n_neg, NULL);

  // 3. entier(...)
  typevaleur v_ent = {.fonction = ENTIER};
  Node *racine = creer_noeud(FONCTION, v_ent, n_abs, NULL);

  // On teste avec 5.6
  // val_neg(5.6) -> -5.6
  // abs(-5.6) -> 5.6
  // entier(5.6) -> 6.0 (arrondi au plus proche selon ta fonction)
  float x_test = 5.6f;
  float res = Eval(racine, x_test);

  printf("Valeur de base  : %.2f\n", x_test);
  printf("Resultat obtenu : %.2f\n", res);
  printf("Resultat attendu: 6.00\n");

  if (res == 6.0f && Eval_get_error() == EVAL_OK) {
    printf(">> Test REUSSI ! \n");
  } else {
    printf(">> Probleme avec l'imbrication ou l'arrondi.\n");
  }

  liberer_arbre(racine);
}

// Test 8 : Intégrale numérique (méthode des trapèzes)
void test_integration() {
  printf(
      "\n--- Test 8 : Integration numerique de f(x) = x entre 0 et 10 ---\n");
  Eval_reset_error();

  // 1. On crée l'arbre qui représente simplement "x"
  typevaleur v_vide = {0};
  Node *racine = creer_noeud(VARIABLE, v_vide, NULL, NULL);

  // 2. On lance l'intégration :
  // On intègre 'racine' de a=0.0 à b=10.0, en découpant en n=1000 morceaux
  float a = 0.0f;
  float b = 10.0f;
  int precision = 1000;

  float res = my_integral(racine, a, b, precision);

  printf("Equation testee  : integrale(x)\n");
  printf("Bornes           : [%.1f, %.1f]\n", a, b);
  printf("Resultat obtenu  : %.2f\n", res);
  printf("Resultat attendu : 50.00\n");

  if (res > 49.9f && res < 50.1f && Eval_get_error() == EVAL_OK) {
    printf(">> Test REUSSI ! \n");
  } else {
    printf(">> Probleme avec le calcul de l'integrale.\n");
  }

  liberer_arbre(racine);
}

// Test 9 : Intégration de f(x) = sin(x) entre 0 et PI
void test_integral_sinus() {
  printf("\n--- Test 9 : Integration de sin(x) entre 0 et PI ---\n");
  Eval_reset_error();

  // 1. On crée l'arbre : sin(x)
  typevaleur v_vide = {0};
  Node *n_x = creer_noeud(VARIABLE, v_vide, NULL, NULL);

  typevaleur v_sin = {.fonction = SIN};
  Node *racine = creer_noeud(FONCTION, v_sin, n_x, NULL);

  // 2. Paramètres de l'intégrale
  float a = 0.0f;
  float b = 3.14159265f; // Approximativement PI
  int n = 1000;          // Nombre de trapèzes

  float res = my_integral(racine, a, b, n);

  printf("Equation testee  : integrale(sin(x))\n");
  printf("Bornes           : [%.2f, %.2f]\n", a, b);
  printf("Resultat obtenu  : %.5f\n", res);
  printf("Resultat attendu : 2.00000\n");

  // On vérifie si on est proche de 2.0
  if (res > 1.99f && res < 2.01f && Eval_get_error() == EVAL_OK) {
    printf(">> Test REUSSI ! \n");
  } else {
    printf(">> Probleme avec l'integrale du sinus (verifie ta constante PI ou "
           "ta fonction my_sin).\n");
  }

  liberer_arbre(racine);
}

// --- Le programme principal ---

int main(void) {
  printf("=== LANCEMENT DE DES TESTS ===\n");

  // On appelle nos tests un par un
  test_expression_simple();
  test_division_par_zero();
  test_identite_trigo_extreme();
  test_exponentielle();
  test_limites_zero();
  test_racine_et_log();
  test_utilitaires();
  test_integration();
  test_integral_sinus();

  printf("\n=== TOUS LES TESTS SONT TERMINES ===\n");
  return 0;
}