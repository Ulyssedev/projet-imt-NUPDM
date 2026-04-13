#include "syntaxique.h"

#include <stdlib.h>

typedef struct {
  const typejeton *entree;
  int index_entree;
  typejeton *sortie;
  int index_sortie;
} ContexteSyntaxe;

static int est_parenthese_fermante(typetoken token) {
  return token == PAR_FERM /*|| token == PAR_FEM */;
}

static typetoken token_courant(const ContexteSyntaxe *ctx) {
  return ctx->entree[ctx->index_entree].lexem;
}

static typejeton jeton_courant(const ContexteSyntaxe *ctx) {
  return ctx->entree[ctx->index_entree];
}

static int analyser_exp(ContexteSyntaxe *ctx);

static int analyser_primaire(ContexteSyntaxe *ctx) {
  typetoken courant = token_courant(ctx);

  if (courant == REEL || courant == VARIABLE) {
    ctx->sortie[ctx->index_sortie++] = jeton_courant(ctx);
    ctx->index_entree++;
    return 1;
  }

  if (courant == FONCTION) {
    typejeton fonction = jeton_courant(ctx);
    ctx->index_entree++;

    if (token_courant(ctx) == PAR_OUV) {
      ctx->index_entree++;

      if (!analyser_exp(ctx)) {
        return 0;
      }

      if (!est_parenthese_fermante(token_courant(ctx))) {
        return 0;
      }

      ctx->index_entree++;
    } else {
      if (!analyser_primaire(ctx)) {
        return 0;
      }
    }

    ctx->sortie[ctx->index_sortie++] = fonction;
    return 1;
  }

  if (courant == PAR_OUV) {
    ctx->index_entree++;

    if (!analyser_exp(ctx)) {
      return 0;
    }

    if (!est_parenthese_fermante(token_courant(ctx))) {
      return 0;
    }

    ctx->index_entree++;
    return 1;
  }

  return 0;
}

static int analyser_puissance(ContexteSyntaxe *ctx) {
  typejeton operateur;

  if (!analyser_primaire(ctx)) {
    return 0;
  }

  if (token_courant(ctx) == OPERATEUR &&
      jeton_courant(ctx).valeur.operateur == PUIS) {
    operateur = jeton_courant(ctx);
    ctx->index_entree++;

    if (!analyser_puissance(ctx)) {
      return 0;
    }

    ctx->sortie[ctx->index_sortie++] = operateur;
  }

  return 1;
}

static int analyser_produit(ContexteSyntaxe *ctx) {
  if (!analyser_puissance(ctx)) {
    return 0;
  }

  while (token_courant(ctx) == OPERATEUR) {
    typejeton operateur = jeton_courant(ctx);

    if (operateur.valeur.operateur != FOIS &&
        operateur.valeur.operateur != DIV) {
      break;
    }

    ctx->index_entree++;

    if (!analyser_puissance(ctx)) {
      return 0;
    }

    ctx->sortie[ctx->index_sortie++] = operateur;
  }

  return 1;
}

static int analyser_exp(ContexteSyntaxe *ctx) {
  if (!analyser_produit(ctx)) {
    return 0;
  }

  while (token_courant(ctx) == OPERATEUR) {
    typejeton operateur = jeton_courant(ctx);

    if (operateur.valeur.operateur != PLUS &&
        operateur.valeur.operateur != MOINS) {
      break;
    }

    ctx->index_entree++;

    if (!analyser_produit(ctx)) {
      return 0;
    }

    ctx->sortie[ctx->index_sortie++] = operateur;
  }

  return 1;
}

int convertir_en_postfixe(typejeton entree[], typejeton sortie[]) {
  ContexteSyntaxe ctx;

  if (entree == 0 || sortie == 0) {
    return SYNTAXE_ERREUR_ARGUMENT;
  }

  ctx.entree = entree;
  ctx.index_entree = 0;
  ctx.sortie = sortie;
  ctx.index_sortie = 0;

  if (!analyser_exp(&ctx)) {
    return SYNTAXE_ERREUR_GRAMMAIRE;
  }

  if (token_courant(&ctx) != FIN) {
    return SYNTAXE_ERREUR_FIN_MANQUANTE;
  }

  ctx.sortie[ctx.index_sortie].lexem = FIN;
  ctx.sortie[ctx.index_sortie].valeur.reel = 0.0f;

  return SYNTAXE_OK;
}

static void liberer_arbre_local(Arbre arbre) {
  if (arbre == 0) {
    return;
  }

  liberer_arbre_local(arbre->pjeton_preced);
  liberer_arbre_local(arbre->pjeton_suiv);
  free(arbre);
}

int convertir_code_postfixe_en_arbre(typejeton code_postfixe[], Arbre *arbre) {
  Arbre *pile;
  int capacite = 0;
  int sommet = -1;
  int i = 0;
  int j;

  if (code_postfixe == 0 || arbre == 0) {
    return SYNTAXE_ERREUR_ARGUMENT;
  }

  *arbre = 0;

  while (code_postfixe[capacite].lexem != FIN) {
    capacite++;
  }

  if (capacite == 0) {
    return SYNTAXE_ERREUR_GRAMMAIRE;
  }

  pile = (Arbre *)malloc((size_t)capacite * sizeof(Arbre));
  if (pile == 0) {
    return SYNTAXE_ERREUR_ARGUMENT;
  }

  while (code_postfixe[i].lexem != FIN) {
    typejeton courant = code_postfixe[i];
    Arbre noeud = (Arbre)malloc(sizeof(Node));

    if (noeud == 0) {
      for (j = 0; j <= sommet; j++) {
        liberer_arbre_local(pile[j]);
      }
      free(pile);
      return SYNTAXE_ERREUR_ARGUMENT;
    }

    noeud->jeton = courant;
    noeud->pjeton_preced = 0;
    noeud->pjeton_suiv = 0;

    if (courant.lexem == REEL || courant.lexem == VARIABLE) {
      if (sommet + 1 >= capacite) {
        free(noeud);
        for (j = 0; j <= sommet; j++) {
          liberer_arbre_local(pile[j]);
        }
        free(pile);
        return SYNTAXE_ERREUR_GRAMMAIRE;
      }

      pile[++sommet] = noeud;
      i++;
      continue;
    }

    if (courant.lexem == FONCTION) {
      if (sommet < 0) {
        free(noeud);
        for (j = 0; j <= sommet; j++) {
          liberer_arbre_local(pile[j]);
        }
        free(pile);
        return SYNTAXE_ERREUR_GRAMMAIRE;
      }

      noeud->pjeton_preced = pile[sommet--];
      pile[++sommet] = noeud;
      i++;
      continue;
    }

    if (courant.lexem == OPERATEUR) {
      Arbre droite;
      Arbre gauche;

      if (sommet < 1) {
        free(noeud);
        for (j = 0; j <= sommet; j++) {
          liberer_arbre_local(pile[j]);
        }
        free(pile);
        return SYNTAXE_ERREUR_GRAMMAIRE;
      }

      droite = pile[sommet--];
      gauche = pile[sommet--];
      noeud->pjeton_preced = gauche;
      noeud->pjeton_suiv = droite;
      pile[++sommet] = noeud;
      i++;
      continue;
    }

    free(noeud);
    for (j = 0; j <= sommet; j++) {
      liberer_arbre_local(pile[j]);
    }
    free(pile);
    return SYNTAXE_ERREUR_GRAMMAIRE;
  }

  if (sommet != 0) {
    for (j = 0; j <= sommet; j++) {
      liberer_arbre_local(pile[j]);
    }
    free(pile);
    return SYNTAXE_ERREUR_GRAMMAIRE;
  }

  *arbre = pile[0];
  free(pile);
  return SYNTAXE_OK;
}
