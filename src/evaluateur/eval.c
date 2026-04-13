#include "eval.h"
#include <stddef.h>

static eval_error_t g_eval_error = EVAL_OK;

void Eval_reset_error(void) { g_eval_error = EVAL_OK; }

eval_error_t Eval_get_error(void) { return g_eval_error; }

// Fonction récursive principale : elle parcourt l'arbre syntaxique (AST) pour
// calculer l'expression finale
float Eval(Arbre A, float x) {
  if (A == NULL) {
    if (g_eval_error == EVAL_OK) {
      g_eval_error = EVAL_ERREUR_ARBRE_NULL;
    }
    return 0.0f;
  }

  switch (A->jeton.lexem) {
  case REEL:
    return A->jeton.valeur.reel;
    break;
  case VARIABLE:
    return x; // C'est ici qu'on injecte la valeur de la variable dans
              // l'équation
    break;
  case FONCTION:
    // On évalue d'abord l'argument (pjeton_preced) avant de lui appliquer la
    // fonction mathématique
    switch (A->jeton.valeur.fonction) {
    case SIN:
      return sinf(
          Eval(A->pjeton_preced, x)); // La fonction maison a été changée
      break;
    case COS:
      return cosf(Eval(A->pjeton_preced, x));
      break;
    case SQRT:
      return sqrtf(Eval(A->pjeton_preced, x));
      break;
    case ABS:
      return my_abs(Eval(A->pjeton_preced, x));
      break;
    case LOG:
      return my_log(Eval(A->pjeton_preced, x));
      break;
    case TAN:
      return my_tan(Eval(A->pjeton_preced, x));
      break;
    case EXP:
      return my_exp(Eval(A->pjeton_preced, x));
      break;
    case ENTIER:
      return entier(Eval(A->pjeton_preced, x));
      break;
    case SINC:
      return sinc(Eval(A->pjeton_preced, x));
      break;
    case VAL_NEG:
      return val_neg(Eval(A->pjeton_preced, x));
      break;
    default:
      if (g_eval_error == EVAL_OK) {
        g_eval_error = EVAL_ERREUR_NOEUD_INVALIDE;
      }
      return 0.0f;
    }
    break;
  case OPERATEUR:
    // Opération binaire : on calcule la branche gauche (preced), la branche
    // droite (suiv), puis on applique l'opérateur
    switch (A->jeton.valeur.operateur) {
    case PLUS:
      return Eval(A->pjeton_preced, x) + Eval(A->pjeton_suiv, x);
      break;
    case MOINS:
      return Eval(A->pjeton_preced, x) - Eval(A->pjeton_suiv, x);
      break;
    case FOIS:
      return Eval(A->pjeton_preced, x) * Eval(A->pjeton_suiv, x);
      break;
    case DIV: {
      float numerateur = Eval(A->pjeton_preced, x);
      float denominateur = Eval(A->pjeton_suiv, x);
      if (denominateur == 0.0f) {
        if (g_eval_error == EVAL_OK) {
          g_eval_error = EVAL_ERREUR_DIVISION_PAR_ZERO;
        }
        return 0.0f;
      }
      return numerateur / denominateur;
    } break;
    case PUIS:
      return pow(Eval(A->pjeton_preced, x), Eval(A->pjeton_suiv, x));
      break;
    default:
      if (g_eval_error == EVAL_OK) {
        g_eval_error = EVAL_ERREUR_NOEUD_INVALIDE;
      }
      return 0.0f;
    }
  default:
    if (g_eval_error == EVAL_OK) {
      g_eval_error = EVAL_ERREUR_NOEUD_INVALIDE;
    }
    return 0.0f;
  }
}

// --- Fonctions mathématiques "maison" ---

float my_cos(float x) {
  /* Réduire l'angle pour améliorer l'approximation (ramener à [-pi, pi]) */
  const float PI = 3.14159265358979323846f;
  const float TWO_PI = 2.0f * PI;
  x = fmodf(x, TWO_PI);
  if (x > PI)
    x -= TWO_PI;
  else if (x < -PI)
    x += TWO_PI;

  int i;
  float res = 1.0f;
  float terme = 1.0f;
  /* Approximation de cos(x) via la série de Taylor autour de 0 */
  for (i = 1; i <= 10; ++i) {
    terme *= -x * x / ((2 * i - 1) * (2 * i));
    res += terme;
  }
  return res;
}

float my_sin(float x) {
  const float PI = 3.14159265358979323846f;
  const float TWO_PI = 2.0f * PI;
  /* Réduire l'angle pour améliorer l'approximation (ramener à [-pi, pi]) */
  x = fmodf(x, TWO_PI);
  if (x < -PI)
    x += TWO_PI;
  if (x > PI)
    x -= TWO_PI;

  float res = x;
  float terme = x; /* premier terme */
  for (int k = 1; k <= 12; ++k) {
    terme *= -x * x / ((2 * k) * (2 * k + 1));
    res += terme;
  }
  return res;
}

float my_sqrt(float x) {
  if (x < 0.0f) {
    return NAN; /* Indéfinie pour des valeurs négatives */
  }
  float precision = 1e-6f;
  float estimation = x;
  float estimation_prec = 0.0f;
  while (fabsf(estimation - estimation_prec) > precision) {
    estimation_prec = estimation;
    estimation = 0.5f * (estimation_prec + x / estimation_prec);
  }
  return estimation;
}

float my_log(float x) {
  // if (x <= 0.0f) {
  //   return -1e38f; /* en dehors des domaines de calcul */
  // }

  // Approximation du logarithme
  // float z = (x - 1.0) / (x+1.0);
  // float terme = z;
  // float res = z;
  // int i;
  // for (i = 1; i <= 15; i++) {
  // terme *= z * z;
  // res += terme / (2 * i + 1);

  // return res / 2.0;

  return logf(x);
}

float my_tan(float x) {
  float c = my_cos(x);
  if (c == 0) { // Protection contre la division par zéro
    return NAN;
  }
  return my_sin(x) / c;
}

float my_exp(float x) {
  // float terme = 1.0f;
  // float res = 1.0f;
  // for (int i = 1; i <= 50; ++i) {
  //   terme *= x / i;
  //   res += terme;
  // }
  // return res;

  return expf(x);
}

float my_abs(float x) {
  // Retourne la valeur absolue
  if (x < 0) {
    return -x;
  }
  return x;
}

float entier(float x) {
  // Arrondi mathématique à l'entier le plus proche
  if ((x + 0.5) > floor(x)) {
    return floor(x) + 1;
  }
  return floor(x);
}

float val_neg(float x) {
  // Force la valeur à être négative
  if (x > 0) {
    return -x;
  }
  return x;
}

float sinc(float x) {
  // Fonction sinus cardinal
  if (x == 0) {
    return 1.0; // Limite usuelle quand x tend vers 0
  }
  return my_sin(x) / x;
}

float my_integral(
    Arbre expression, float a, float b,
    int n) { // fonction intégrale ajouter en plus (pour le plaisir)
  if (n <= 0 || expression == NULL) {
    // Petite sécurité
    if (g_eval_error == EVAL_OK && expression == NULL) {
      g_eval_error = EVAL_ERREUR_ARBRE_NULL;
    }
    return 0.0f;
  }

  float h = (b - a) / n; // h est la largeur de chaque petit trapèze

  // Formule des trapèzes
  float f_a = Eval(expression, a);
  float f_b =
      Eval(expression, b); // h * [ (f(a) + f(b))/2 + f(a+h) + f(a+2h) + ... ]

  float somme = (f_a + f_b) / 2.0f;

  // On calcule les points intermédiaires
  int i;
  for (i = 1; i < n; i++) {
    float x_actuel = a + i * h;
    somme += Eval(expression, x_actuel);
  }

  return somme * h;
}