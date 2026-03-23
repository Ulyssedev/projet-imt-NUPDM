#include "syntaxique.h"

typedef struct {
  const typejeton *entree;
  int index_entree;
  typejeton *sortie;
  int index_sortie;
} ContexteSyntaxe;

static int est_parenthese_fermante(typetoken token) {
  return token == PAR_FERM || token == PAR_FEM;
}

static typetoken token_courant(const ContexteSyntaxe *ctx) {
  return ctx->entree[ctx->index_entree].lexem;
}

static typejeton jeton_courant(const ContexteSyntaxe *ctx) {
  return ctx->entree[ctx->index_entree];
}

static int analyser_exp(ContexteSyntaxe *ctx) {
  typetoken courant = token_courant(ctx);

  if (courant == REEL || courant == VARIABLE) {
    ctx->sortie[ctx->index_sortie++] = jeton_courant(ctx);
    ctx->index_entree++;
    return 1;
  }

  if (courant == FONCTION) {
    typejeton fonction = jeton_courant(ctx);
    ctx->index_entree++;

    if (!analyser_exp(ctx)) {
      return 0;
    }

    ctx->sortie[ctx->index_sortie++] = fonction;
    return 1;
  }

  if (courant == PAR_OUV) {
    typejeton operateur;

    ctx->index_entree++;

    if (!analyser_exp(ctx)) {
      return 0;
    }

    courant = token_courant(ctx);

    if (courant == OPERATEUR) {
      operateur = jeton_courant(ctx);
      ctx->index_entree++;

      if (!analyser_exp(ctx)) {
        return 0;
      }

      if (!est_parenthese_fermante(token_courant(ctx))) {
        return 0;
      }

      ctx->index_entree++;
      ctx->sortie[ctx->index_sortie++] = operateur;
      return 1;
    }

    if (est_parenthese_fermante(courant)) {
      ctx->index_entree++;
      return 1;
    }

    return 0;
  }

  return 0;
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

  return SYNTAXE_OK;
}

// TODO: faire une vraie implémentation de cette fonction car c'est la syntaxe
// qui s'en occupe ;( ;( ;(
int convertir_code_postfixe_en_arbre(typejeton code_postfixe[], Arbre *arbre) {
  (void)code_postfixe;
  (void)arbre;
  return SYNTAXE_OK;
}
