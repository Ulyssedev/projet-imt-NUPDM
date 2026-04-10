// Définition des différents tokens utilisés lors du projet

typedef enum
{
  // ERREUR,
  REEL,
  OPERATEUR,
  FONCTION,
  FIN,
  PAR_OUV,
  PAR_FEM,
  PAR_FERM,
  VARIABLE,
  BAR_OUV, // Wtf is this ?? codialement KB
  BAR_FERM,
  ABSOLU
} typetoken;

typedef enum { PLUS, MOINS, FOIS, DIV, PUIS } typeoperateur;

typedef enum {
  ABS,
  SIN,
  SQRT,
  LOG,
  COS,
  TAN,
  EXP,
  ENTIER,
  VAL_NEG,
  SINC
} typefonction;

// possibly wrong place
// typedef enum {
//   DIVISER_PAR_ZERO,
//   FUN_NO_RECO,
//   VAR_NO_RECO,
//   ARG_MANQUANT
// } typeerreur;

typedef union {
  float reel;
  typefonction fonction;
  typeoperateur operateur;
  // typeerreur erreur;
} typevaleur;

typedef struct {
  typetoken lexem;
  typevaleur valeur;
} typejeton;

typedef struct Node {
  typejeton jeton;
  struct Node *pjeton_preced;
  struct Node *pjeton_suiv;
} Node;

typedef Node *Arbre;
