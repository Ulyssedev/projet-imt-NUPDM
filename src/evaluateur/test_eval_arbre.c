/**
 * test_eval_arbre.c — Tests unitaires de Eval() via des arbres construits à la main
 *
 * Compilation : gcc test_eval_arbre.c eval.c -o test_eval -lm
 * Exécution   : ./test_eval
 *
 * Types exacts issus de jeton.h / eval.h.
 */

#include <stdio.h>
#include <math.h>
#include "eval.h"

/* ================================================================
   --- Helpers : construction de noeuds ---
   ================================================================ */

Node noeud_reel(float v) {
    Node n;
    n.jeton.lexem       = REEL;
    n.jeton.valeur.reel = v;
    n.pjeton_preced     = NULL;
    n.pjeton_suiv       = NULL;
    return n;
}

Node noeud_var() {
    Node n;
    n.jeton.lexem   = VARIABLE;
    n.pjeton_preced = NULL;
    n.pjeton_suiv   = NULL;
    return n;
}

Node noeud_op(typeoperateur op, Arbre gauche, Arbre droite) {
    Node n;
    n.jeton.lexem            = OPERATEUR;
    n.jeton.valeur.operateur = op;
    n.pjeton_preced          = gauche;
    n.pjeton_suiv            = droite;
    return n;
}

Node noeud_fn(typefonction f, Arbre arg) {
    Node n;
    n.jeton.lexem           = FONCTION;
    n.jeton.valeur.fonction = f;
    n.pjeton_preced         = arg;
    n.pjeton_suiv           = NULL;
    return n;
}

/*
 * Eval attend un Arbre* (= Node**).
 * La macro crée un Arbre temporaire pointant sur le Node,
 * puis appelle Eval avec l'adresse de cet Arbre.
 */
#define EVAL(noeud, xval) \
    ({ Arbre _tmp = &(noeud); Eval(&_tmp, (xval)); })

/* ================================================================
   --- Mini-framework de test ---
   ================================================================ */
static int total  = 0;
static int passes = 0;
static int echecs = 0;

#define TOL 0.001f

void verifier(const char *desc, float obtenu, float attendu, float tol) {
    total++;
    float diff = (float)fabs((double)(obtenu - attendu));
    if (diff <= tol) {
        printf("  [PASS] %-48s =>  %.5f\n", desc, obtenu);
        passes++;
    } else {
        printf("  [FAIL] %-48s =>  %.5f  (attendu %.5f, ecart %.5f)\n",
               desc, obtenu, attendu, diff);
        echecs++;
    }
}

void section(const char *titre) {
    printf("\n=== %s ===\n", titre);
}

/* ================================================================
   --- Tests ---
   ================================================================ */

void test_feuilles() {
    section("Feuilles : REEL et VARIABLE");

    Node n3   = noeud_reel(3.0f);
    Node nvar = noeud_var();

    verifier("REEL(3.0)  ignore x=0",  EVAL(n3,    0.0f),   3.0f, TOL);
    verifier("REEL(3.0)  ignore x=99", EVAL(n3,   99.0f),   3.0f, TOL);
    verifier("VARIABLE   x=5",         EVAL(nvar,  5.0f),   5.0f, TOL);
    verifier("VARIABLE   x=-2.5",      EVAL(nvar, -2.5f),  -2.5f, TOL);
    verifier("VARIABLE   x=0",         EVAL(nvar,  0.0f),   0.0f, TOL);
}

void test_operateurs() {
    section("Operateurs binaires (constantes uniquement)");

    Node a, b, op;

    a = noeud_reel(2.0f);   b = noeud_reel(3.0f);
    op = noeud_op(PLUS,  &a, &b);
    verifier("2 + 3 = 5",       EVAL(op, 0.0f),    5.0f, TOL);

    a = noeud_reel(7.0f);   b = noeud_reel(4.0f);
    op = noeud_op(MOINS, &a, &b);
    verifier("7 - 4 = 3",       EVAL(op, 0.0f),    3.0f, TOL);

    a = noeud_reel(6.0f);   b = noeud_reel(5.0f);
    op = noeud_op(FOIS,  &a, &b);
    verifier("6 * 5 = 30",      EVAL(op, 0.0f),   30.0f, TOL);

    a = noeud_reel(10.0f);  b = noeud_reel(4.0f);
    op = noeud_op(DIV,   &a, &b);
    verifier("10 / 4 = 2.5",    EVAL(op, 0.0f),    2.5f, TOL);

    a = noeud_reel(2.0f);   b = noeud_reel(10.0f);
    op = noeud_op(PUIS,  &a, &b);
    verifier("2 ^ 10 = 1024",   EVAL(op, 0.0f), 1024.0f, TOL);

    a = noeud_reel(3.0f);   b = noeud_reel(0.0f);
    op = noeud_op(PUIS,  &a, &b);
    verifier("3 ^ 0 = 1",       EVAL(op, 0.0f),    1.0f, TOL);
}

void test_avec_variable() {
    section("Expressions mixtes avec la variable x");

    Node var, cst, expr;

    /* x + 1 */
    var = noeud_var();  cst = noeud_reel(1.0f);
    expr = noeud_op(PLUS,  &var, &cst);
    verifier("x+1  (x=4)  = 5",   EVAL(expr,  4.0f),  5.0f, TOL);
    verifier("x+1  (x=0)  = 1",   EVAL(expr,  0.0f),  1.0f, TOL);
    verifier("x+1  (x=-3) = -2",  EVAL(expr, -3.0f), -2.0f, TOL);

    /* 2 * x */
    var = noeud_var();  cst = noeud_reel(2.0f);
    expr = noeud_op(FOIS,  &cst, &var);
    verifier("2*x  (x=3)  = 6",   EVAL(expr,  3.0f),  6.0f, TOL);
    verifier("2*x  (x=0)  = 0",   EVAL(expr,  0.0f),  0.0f, TOL);

    /* x ^ 2 */
    var = noeud_var();  cst = noeud_reel(2.0f);
    expr = noeud_op(PUIS,  &var, &cst);
    verifier("x^2  (x=5)  = 25",  EVAL(expr,  5.0f), 25.0f, TOL);
    verifier("x^2  (x=-3) = 9",   EVAL(expr, -3.0f),  9.0f, TOL);

    /* x / 2 */
    var = noeud_var();  cst = noeud_reel(2.0f);
    expr = noeud_op(DIV,   &var, &cst);
    verifier("x/2  (x=8)  = 4",   EVAL(expr,  8.0f),  4.0f, TOL);
}

void test_fonctions_constantes() {
    section("Fonctions unaires sur constantes");

    float pi = 3.14159265f;
    Node arg, fn;

    arg = noeud_reel(0.0f);       fn = noeud_fn(SIN,     &arg);
    verifier("sin(0) = 0",           EVAL(fn, 0.0f),  0.0f,     TOL);

    arg = noeud_reel(pi / 2.0f);  fn = noeud_fn(SIN,     &arg);
    verifier("sin(pi/2) = 1",        EVAL(fn, 0.0f),  1.0f,     TOL);

    arg = noeud_reel(0.0f);       fn = noeud_fn(COS,     &arg);
    verifier("cos(0) = 1",           EVAL(fn, 0.0f),  1.0f,     TOL);

    arg = noeud_reel(pi / 4.0f);  fn = noeud_fn(COS,     &arg);
    verifier("cos(pi/4) ~ 0.707",    EVAL(fn, 0.0f),  0.70711f, TOL);

    arg = noeud_reel(9.0f);       fn = noeud_fn(SQRT,    &arg);
    verifier("sqrt(9) = 3",          EVAL(fn, 0.0f),  3.0f,     TOL);

    arg = noeud_reel(-1.0f);      fn = noeud_fn(SQRT,    &arg);
    verifier("sqrt(-1) => 0 (guard)",EVAL(fn, 0.0f),  0.0f,     TOL);

    arg = noeud_reel(-5.0f);      fn = noeud_fn(ABS,     &arg);
    verifier("abs(-5) = 5",          EVAL(fn, 0.0f),  5.0f,     TOL);

    arg = noeud_reel(2.71828f);   fn = noeud_fn(LOG,     &arg);
    verifier("log(e) ~ 1",           EVAL(fn, 0.0f),  1.0f,     TOL);

    arg = noeud_reel(pi / 4.0f);  fn = noeud_fn(TAN,     &arg);
    verifier("tan(pi/4) ~ 1",        EVAL(fn, 0.0f),  1.0f,     TOL);

    arg = noeud_reel(0.0f);       fn = noeud_fn(SINC,    &arg);
    verifier("sinc(0) = 1",          EVAL(fn, 0.0f),  1.0f,     TOL);

    arg = noeud_reel(pi);         fn = noeud_fn(SINC,    &arg);
    verifier("sinc(pi) ~ 0",         EVAL(fn, 0.0f),  0.0f,     TOL);

    arg = noeud_reel(5.0f);       fn = noeud_fn(VAL_NEG, &arg);
    verifier("val_neg(5) = -5",      EVAL(fn, 0.0f), -5.0f,     TOL);

    arg = noeud_reel(-3.0f);      fn = noeud_fn(VAL_NEG, &arg);
    verifier("val_neg(-3) = -3",     EVAL(fn, 0.0f), -3.0f,     TOL);

    arg = noeud_reel(2.7f);       fn = noeud_fn(ENTIER,  &arg);
    verifier("entier(2.7) = 3",      EVAL(fn, 0.0f),  3.0f,     TOL);

    arg = noeud_reel(2.3f);       fn = noeud_fn(ENTIER,  &arg);
    verifier("entier(2.3) = 2",      EVAL(fn, 0.0f),  2.0f,     TOL);
}

void test_fonctions_sur_variable() {
    section("Fonctions unaires appliquees a x");

    float pi = 3.14159265f;
    Node var, fn;

    var = noeud_var();  fn = noeud_fn(SIN,  &var);
    verifier("sin(x) (x=pi/2) = 1",  EVAL(fn, pi/2.0f),  1.0f, TOL);
    verifier("sin(x) (x=0)    = 0",  EVAL(fn, 0.0f),     0.0f, TOL);

    var = noeud_var();  fn = noeud_fn(SQRT, &var);
    verifier("sqrt(x) (x=16) = 4",   EVAL(fn, 16.0f),    4.0f, TOL);
    verifier("sqrt(x) (x=1)  = 1",   EVAL(fn,  1.0f),    1.0f, TOL);

    var = noeud_var();  fn = noeud_fn(ABS,  &var);
    verifier("abs(x)  (x=-7) = 7",   EVAL(fn, -7.0f),    7.0f, TOL);
    verifier("abs(x)  (x=4)  = 4",   EVAL(fn,  4.0f),    4.0f, TOL);
}

void test_arbres_composites() {
    section("Arbres composites (expressions imbriquees)");

    float pi = 3.14159265f;

    /* sin²(x) + cos²(x) = 1 */
    {
        Node vs = noeud_var(), vc = noeud_var();
        Node e2s = noeud_reel(2.0f), e2c = noeud_reel(2.0f);
        Node sn  = noeud_fn(SIN,  &vs);
        Node cs  = noeud_fn(COS,  &vc);
        Node sn2 = noeud_op(PUIS, &sn,  &e2s);
        Node cs2 = noeud_op(PUIS, &cs,  &e2c);
        Node id  = noeud_op(PLUS, &sn2, &cs2);
        verifier("sin^2+cos^2=1  (x=1)", EVAL(id, 1.0f), 1.0f, TOL);
        verifier("sin^2+cos^2=1  (x=3)", EVAL(id, 3.0f), 1.0f, TOL);
    }

    /* sqrt(x^2) = |x| */
    {
        Node var  = noeud_var();
        Node deux = noeud_reel(2.0f);
        Node x2   = noeud_op(PUIS, &var, &deux);
        Node sqx  = noeud_fn(SQRT, &x2);
        verifier("sqrt(x^2) (x=7)  = 7",  EVAL(sqx,  7.0f),  7.0f, TOL);
        verifier("sqrt(x^2) (x=3)  = 3",  EVAL(sqx,  3.0f),  3.0f, TOL);
    }

    /* abs(x - 10) */
    {
        Node var  = noeud_var();
        Node dix  = noeud_reel(10.0f);
        Node diff = noeud_op(MOINS, &var,  &dix);
        Node ab   = noeud_fn(ABS,   &diff);
        verifier("abs(x-10) (x=3)  = 7",  EVAL(ab,  3.0f),  7.0f, TOL);
        verifier("abs(x-10) (x=15) = 5",  EVAL(ab, 15.0f),  5.0f, TOL);
        verifier("abs(x-10) (x=10) = 0",  EVAL(ab, 10.0f),  0.0f, TOL);
    }

    /* (x+2)*(x-2) = x²-4 */
    {
        Node va  = noeud_var(),     vb  = noeud_var();
        Node n2a = noeud_reel(2.0f),n2b = noeud_reel(2.0f);
        Node xp2 = noeud_op(PLUS,  &va,  &n2a);
        Node xm2 = noeud_op(MOINS, &vb,  &n2b);
        Node prd = noeud_op(FOIS,  &xp2, &xm2);
        verifier("(x+2)*(x-2) (x=5) = 21", EVAL(prd, 5.0f), 21.0f, TOL);
        verifier("(x+2)*(x-2) (x=2) = 0",  EVAL(prd, 2.0f),  0.0f, TOL);
        verifier("(x+2)*(x-2) (x=0) = -4", EVAL(prd, 0.0f), -4.0f, TOL);
    }

    /* sinc(pi*x) */
    {
        Node pi_n = noeud_reel(pi);
        Node var  = noeud_var();
        Node pix  = noeud_op(FOIS, &pi_n, &var);
        Node sc   = noeud_fn(SINC, &pix);
        verifier("sinc(pi*x) (x=0) = 1",   EVAL(sc, 0.0f),  1.0f, TOL);
        verifier("sinc(pi*x) (x=1) = 0",   EVAL(sc, 1.0f),  0.0f, TOL);
        verifier("sinc(pi*x) (x=2) = 0",   EVAL(sc, 2.0f),  0.0f, TOL);
    }
}

/* ================================================================
   --- Point d'entree ---
   ================================================================ */
int main(void) {
    printf("+==================================================+\n");
    printf("|    TESTS UNITAIRES  --  Eval(Arbre*, x)         |\n");
    printf("+==================================================+\n");

    test_feuilles();
    test_operateurs();
    test_avec_variable();
    test_fonctions_constantes();
    test_fonctions_sur_variable();
    test_arbres_composites();

    printf("\n==================================================\n");
    printf("  %d/%d tests passes", passes, total);
    if (echecs > 0)
        printf("  (%d ECHEC(S))\n", echecs);
    else
        printf("  -- tout au vert !\n");
    printf("==================================================\n");
    return echecs == 0 ? 0 : 1;
}
