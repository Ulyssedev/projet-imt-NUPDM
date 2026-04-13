#ifndef EVAL_H
#define EVAL_H

#include "../common/jeton.h"
#include <math.h>
#include <stddef.h>

typedef enum {
  EVAL_OK = 0,
  EVAL_ERREUR_ARBRE_NULL = 1,
  EVAL_ERREUR_DIVISION_PAR_ZERO = 2,
  EVAL_ERREUR_LOG_NON_POSITIF = 3,
  EVAL_ERREUR_SQRT_NEGATIF = 4,
  EVAL_ERREUR_NOEUD_INVALIDE = 5
} eval_error_t;

float Eval(Arbre A, float x);
void Eval_reset_error(void);
eval_error_t Eval_get_error(void);

float my_cos(float x);
float my_sin(float x);
float my_sqrt(float x);
float my_log(float x);
float my_tan(float x);
float my_exp(float x);
float my_abs(float x);
float entier(float x);
float val_neg(float x);
float sinc(float x);
float my_integral(Arbre expression, float a, float b, int n);

#endif // EVAL_H
