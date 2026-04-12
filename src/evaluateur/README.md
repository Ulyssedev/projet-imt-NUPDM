# Module Evaluateur

## Objectif du module

Le module évaluateur parcourt l'arbre syntaxique produit par le module syntaxique pour calculer la valeur numérique de l'expression. Il permet:

- l'injection de la variable : subsitution du jeton `VARIABLE` par une valeur réelle x donnée

- la résolution de l'expression : parcours de l'arbre de manière récursive

- la gestion des erreurs

## Choix de conception

### 1 : Parcours de l'arbre

L'évaluation repose sur une fonction récursive `Eval`.

Raisonnement : 

La struture de l'arbre impose un parcours de type post-fixé. On évalue d'abord les enfants (les opérandes) avant de traiter le parent (opérateur ou fonction)

Principe :

Un `switch` principal traite le type de jeton (`REEL`, `JETON`, `FONCTION`, `OPERATEUR`). Puis pour le type d'opérateur et la fonction on utilise un autre `switch`. Pour les opérateurs, la fonction s'appelle récursivement sur les branches gauche (`pjeton_preced`) et droite (`pjeton_suiv`).

### 2 : Implémentation des fonctions mathématiques

Réimplémentation des fonctions usuelles:

- Séries de Taylor : Utilisées pour `my_sin`, `my_cos` et `my_exp` avec 10 à 15 itérations afin d'être assez précis

- Méthode de Héron : Utilisée pour `my_sqrt` 

- Logarithme : Approximation par développement en série avec 15 itérations pour un résultat précis

- Définition mathématique : Utilisée pour `sinc` et `val_neg`

## Gestion des erreurs 

La gestion des erreurs est centralisée via une variable globale statique `g_eval_error` et des fonctions d'accès (`Eval_get_error`, `Eval_reset_error`).

Les erreurs détectées sont: 

- `EVAL_ERREUR_ARBRE_NULL` : Tentative d'évaluation d'un arbre inexistant
- `EVAL_ERREUR_DIVISION_PAR_ZERO` : Détectée lors d'une division par zéro
- `EVAL_ERREUR_LOG_NON_POSITIF` : Tentative de calculer ln(x) avec un x négatif
- `EVAL_ERREUR_SQRT_NEGATIF` : Racine carrée d'un nombre strictement négatif 
- `EVAL_ERREUR_NOEUD_INVALIDE` : Présence d'un jeton non reconnu dans la structure de l'arbre

## Fichier du module 

- `eval.h` : définit l'énumération `eval_error_t` et les prototypes de fonction
- `eval.c` : contient la logique d'évaluation récursive et les algorithmes numériques

## Tests 







