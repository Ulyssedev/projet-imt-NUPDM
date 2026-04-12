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
  if (A == NULL) return;
  liberer_arbre(A->pjeton_preced);
  liberer_arbre(A->pjeton_suiv);
  free(A);
}

// --- Nos tests ---

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

  // On vérifie que le calcul est bon et qu'il n'y a pas eu d'erreur déclenchée pendant le parcours
  if (resultat == 11.0f && Eval_get_error() == EVAL_OK) {
    printf(">> Test REUSSI !\n");
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
  
  // La fonction Eval aurait dû lever l'erreur spécifique dans la variable globale
  if (Eval_get_error() == EVAL_ERREUR_DIVISION_PAR_ZERO) {
    printf(">> Test REUSSI ! L'erreur a ete interceptee.\n");
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

  // Le résultat ne sera jamais *parfaitement* 1.0 à cause de la précision des floats,
  // donc on laisse une petite marge d'erreur logique
  if (res > 0.99f && res < 1.01f) {
    printf(">> Test REUSSI ! Taylor fonctionne bien.\n");
  } else {
    printf(">> Enorme probleme, le resultat n'est pas 1.\n");
  }

  liberer_arbre(racine);
}

//Test 4 : La fonction exponentielle 

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

  // On vérifie qu'on est bien autour de 2.718 (on laisse une petite marge pour les floats)
  if (res > 2.71f && res < 2.72f && Eval_get_error() == EVAL_OK) {
    printf(">> Test REUSSI ! La serie de Taylor de exp() est super precise.\n");
  } else {
    printf(">> Enorme probleme, l'approximation est fausse (verifie ta boucle dans my_exp).\n");
  }

  liberer_arbre(racine);
}


// --- Le programme principal ---

int main(void) {
  printf("=== LANCEMENT DE LA BATTERIE DE TESTS ===\n");

  // On appelle nos tests un par un
  test_expression_simple();
  test_division_par_zero();
  test_identite_trigo_extreme();
  test_exponentielle();

  printf("\n=== TOUS LES TESTS SONT TERMINES ===\n");
  return 0;
}