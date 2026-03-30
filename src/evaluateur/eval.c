#include "eval.h"
#include "jeton.h"
#include <math.h>

// Fonction récursive principale : elle parcourt l'arbre syntaxique (AST) pour calculer l'expression finale
float Eval(Arbre A, float x){
    switch(A->jeton.lexem)
    {
        case REEL : 
            return A->jeton.valeur.reel;
        break;
        case VARIABLE :
            return x; // C'est ici qu'on injecte la valeur de la variable dans l'équation
        break;
        case FONCTION:
            // On évalue d'abord l'argument (pjeton_preced) avant de lui appliquer la fonction mathématique
            switch(A->jeton.valeur.fonction)
            {
                case SIN: return sin(Eval(A->pjeton_preced,x)); break;
                case COS: return cos(Eval(A->pjeton_preced,x)); break;
                case SQRT: return sqrt(Eval(A->pjeton_preced,x)); break;
                case ABS: return abs(Eval(A->pjeton_preced,x)); break;
                case LOG: return log(Eval(A->pjeton_preced,x)); break;
                case TAN: return tan(Eval(A->pjeton_preced,x)); break;
                case EXP: return exp(Eval(A->pjeton_preced,x)); break;
                case ENTIER: return entier(Eval(A->pjeton_preced,x)); break;
                case SINC : return sinc(Eval(A->pjeton_preced,x)); break;
                case VAL_NEG : return val_neg(Eval(A->pjeton_preced,x)); break;
            }
        break; 
        case OPERATEUR :
            // Opération binaire : on calcule la branche gauche (preced), la branche droite (suiv), puis on applique l'opérateur
            switch(A->jeton.valeur.operateur)
            {
                case PLUS : return Eval(A->pjeton_preced,x) + Eval(A->pjeton_suiv,x); break;
                case MOINS : return Eval(A->pjeton_preced,x) - Eval(A->pjeton_suiv,x); break;
                case FOIS : return Eval(A->pjeton_preced,x) * Eval(A->pjeton_suiv,x); break;
                case DIV : return Eval(A->pjeton_preced,x) / Eval(A->pjeton_suiv,x); break;
                case PUIS : 
                    return pow(Eval(A->pjeton_preced,x), Eval(A->pjeton_suiv,x));
                break;
            }
    }
}

// --- Fonctions mathématiques "maison" ---

float cos(float x){
    int i;
    float res = 1.0;
    float terme = 1.0;
    // Approximation de cos(x) via la série de Taylor
    for (i=1;i<=10;i=i+1){
        terme *= -x * x / ((2 * i - 1) * (2 * i));
        res += terme;
    }
    return res;
}

float sin(float x){
    int i;
    float res = x;
    float terme = 1.0;
    // Approximation de sin(x) via la série de Taylor
    for (i=1;i<=10;i++){
        terme *= -x * x / ((2*i) * (2*i+1));
        res += terme;
    }
    return res;
}

float sqrt(float x){
    if (x<=0){
        return 0; // Sécurité de base
    }
    float precision = 0.00001;
    float estimation = x;
    float estimation_prec = 0; 
    
    // Calcul de la racine carrée par la méthode de Héron 
    while (estimation - estimation_prec > precision){
        estimation_prec = estimation;
        estimation = (estimation_prec + x/estimation_prec)/2.0;
    }
    return estimation;
}

float log(float x){
    if (x<1){
        return -1e38; // Valeur très basse pour simuler -l'infini en évitant le crash
    }
    // Approximation du logarithme
    float z = (x-1.0)/(x+1.0);
    float terme = z;
    float res = z;
    int i;
    for (i=1;i<=15;i++){
        terme *= z*z;
        res += terme/(2*i+1);
    }
    return res/2.0;
}

float tan(float x){
    float c = cos(x);
    if (c == 0){ // Protection contre la division par zéro
        return x;
    }
    return sin(x)/c;
}

float exp(float x){
    int i;
    if (x==0){
        return 1.0;
    }
    float terme =1.0;
    float res= 1.0;
    // Approximation de e^x via la série de Taylor
    for (i=1;i<=15;i++){
        terme *= -x;
        res += terme/(i+1);
    }
    return res;
}

float abs(float x){
    // Retourne la valeur absolue
    if (x<0){
        return -x;
    }
    return x;
}

float entier(float x){
    // Arrondi mathématique à l'entier le plus proche
    if ((x + 0.5)> floor(x)){
        return floor(x) +1;
    }
    return floor(x);
}

float val_neg(float x){
    // Force la valeur à être négative
    if (x>0){
        return -x;
    }
    return x;
}

float sinc(float x){
    // Fonction sinus cardinal
    if (x==0){
        return 1.0; // Limite usuelle quand x tend vers 0
    }
    return sin(x)/x;
}
