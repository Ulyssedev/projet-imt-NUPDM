#ifndef SYNTAXIQUE_H
#define SYNTAXIQUE_H

#include "../common/jeton.h"

enum {
  SYNTAXE_OK = 0,
  SYNTAXE_ERREUR_ARGUMENT = 1,
  SYNTAXE_ERREUR_GRAMMAIRE = 2,
  SYNTAXE_ERREUR_FIN_MANQUANTE = 3
};

int convertir_en_postfixe(typejeton entree[], typejeton sortie[]);

#endif
