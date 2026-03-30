#include "eval.h"
#include "jeton.h"

float Eval(Arbre A, float x){
    switch(A->jeton.lexem)
    {
        case REEL : 
            return A->jeton.valeur.reel;
        break;
        case VARIABLE :
            return x;
        break;
        case FONCTION:
            switch(A->jeton.valeur.fonction)
            {
                case SIN:
                    return sin(Eval(A->fg,x));
                break;
                case COS:
                    return cos(Eval(A->fg,x));
                break;
                case SQRT:
                    return sqrt(Eval(A->fg,x));
                break;
                case ABS:
                    return abs(Eval(A->fg,x));
                break;
                case LOG:
                    return log(Eval(A->fg,x));
                    break;
                case TAN:
                    return tan(Eval(A->fg,x));
                    break;
                case EXP:
                    return exp(Eval(A->fg,x));
                break;
                case ENTIER:
                    return entier(Eval(A->fg,x));
                break;
                case SINC :
                    return sinc(Eval(A->fg,x));
                break;
                case VAL_NEG :
                    return val_neg(Eval(A->fg,x));
                break;
            }
        break; 
        case OPERATEUR :
            switch(A->jeton.valeur.operateur)
            {
                case PLUS :
                    return Eval(A->fg,x) + Eval(A->fd,x);
                break;
                case MOINS :
                    return Eval(A->fg,x) - Eval(A->fd,x);
                break;
                case FOIS :
                    return Eval(A->fg,x) * Eval(A->fd,x);
                break;
                case DIV :
                    return Eval(A->fg,x) / Eval(A->fd,x);
                break;
                case PUIS :
                    return Eval(A->fg,x)**(Eval(A->fd,x));
                break;
            }
    }
}


float cos(float x){
    int i;
    float res = 1.0;
    float terme = 1.0;
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
    for (i=1;i<=10;i++){
        terme *= -x * x / ((2*i) * (2*i+1));
        res += terme;
    }
}

float sqrt(float x){
    if (x<=0){
        return 0; //erreur
    }
    int precision = 0.00001;
    float estimation = x;
    float estimation_prec;
    while (estimation - estimation_prec > precision){
        estimation_prec = estimation;
        estimation = (estimation_prec + x/estimation_prec)/2.0;
    }
    return estimation;
}

float log(float x){
    if (x<1){
        return -1e38;
    }
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
    if (c == 0){ //ERREUR
    return x;
    }
    return sin(x)/c;
}

float exp(float x){
    int i;
    if (x=0){
        return 1.0;
    }
    float terme =1.0;
    float res= 1.0;
    for (i=1;i<=15;i++){
        terme *= -x;
        res += terme/(i+1);
    }
    return res;
}

float abs(float x){
    if (x<0){
        return -x;
    }
    return x;
}

float entier(float x){
    if ((x + 0.5)> floor(x)){
        return floor(x) +1;
    }
    return floor(x);
}

float val_neg(float x){
    if (x>0){
        return -x;
    }
    return x;
}

float sinc(float x){
    if (x==0){
        return 1.0;
    }
    return sin(x)/x;
}

