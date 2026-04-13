// Définitions des jetons et types auxiliaires utilisés par le lexer,
// l'analyse syntaxique et l'évaluateur.
//
// Ce fichier centralise :
//  - `typetoken` : la catégorie d'un jeton (nombre, opérateur, fonction, etc.)
//  - `typeoperateur` : liste des opérateurs binaires supportés
//  - `typefonction` : fonctions unaire supportées (sin, cos, ...)
//  - `typevaleur` : union contenant la valeur d'un jeton
//  - `typejeton` : structure représentant un jeton complet
//  - `Node` / `Arbre` : noeud d'arbre syntaxique utilisé pour l'AST

#ifndef JETON_H
#define JETON_H

// Catégorie de token rencontrés lors de l'analyse lexicale
typedef enum {
  ERREUR,    // jeton invalide
  REEL,      // nombre réel
  OPERATEUR, // opérateur binaire (+ - * / ^)
  FONCTION,  // fonction unaire (sin, cos, ...)
  FIN,       // marqueur de fin de séquence
  PAR_OUV,   // parenthèse ouvrante '('
  PAR_FEM,   // parenthèse fermante (ancienne variante)
  PAR_FERM,  // parenthèse fermante ')'
  VARIABLE,  // variable (ex: 'x')
  BAR_OUV,   // barre ouvrante '|' (pour valeur absolue)
  BAR_FERM,  // barre fermante '|' (pour valeur absolue)
  ABSOLU     // token interne pour valeur absolue
} typetoken;

// Opérateurs binaires pris en charge
typedef enum { PLUS, MOINS, FOIS, DIV, PUIS } typeoperateur;

// Fonctions mathématiques supportées (utilisées comme jetons FONCTION)
typedef enum {
  ABS,
  SIN,
  SQRT,
  LOG,
  COS,
  TAN,
  EXP,
  ENTIER,  // arrondi/partie entière
  VAL_NEG, // négation unaire (implémentée comme fonction interne)
  SINC
} typefonction;

// Codes d'erreur potentiels (réservés pour l'évaluateur/lexer si nécessaire)
typedef enum {
  DIVISER_PAR_ZERO,
  FUN_NO_RECO,
  VAR_NO_RECO,
  ARG_MANQUANT
} typeerreur;

// Valeur associée à un jeton : soit un réel, soit un identifiant de
// fonction ou d'opérateur selon le lexem.
typedef union {
  float reel;
  typefonction fonction;
  typeoperateur operateur;
  // typeerreur erreur; optionnel, réservé si l'on veut propager des erreurs
} typevaleur;

// Jeton complet : catégorie + valeur
typedef struct {
  typetoken lexem;
  typevaleur valeur;
} typejeton;

// Noeud d'arbre pour représenter l'AST. `pjeton_preced` correspond à la
// branche gauche / argument unique, `pjeton_suiv` à la branche droite si
// applicable.
typedef struct Node {
  typejeton jeton;
  struct Node *pjeton_preced;
  struct Node *pjeton_suiv;
} Node;

typedef Node *Arbre;

#endif // JETON_H