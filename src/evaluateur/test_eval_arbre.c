#include <stdio.h>
#include <stdlib.h>
#include "eval.h"

// Petite fonction maison pour créer un nœud rapidement et proprement
Node* creer_noeud(typetoken lexem, typevaleur val, Node* gauche, Node* droite) {
    Node* nouveau = (Node*)malloc(sizeof(Node));
    if (nouveau == NULL) {
        printf("Erreur d'allocation mémoire !\n");
        exit(1);
    }
    nouveau->jeton.lexem = lexem;
    nouveau->jeton.valeur = val;
    nouveau->pjeton_preced = gauche; // Branche gauche (ou argument pour une fonction)
    nouveau->pjeton_suiv = droite;   // Branche droite
    return nouveau;
}

// Fonction pour libérer la mémoire de l'arbre une fois qu'on a fini
void liberer_arbre(Arbre A) {
    if (A == NULL) return;
    liberer_arbre(A->pjeton_preced);
    liberer_arbre(A->pjeton_suiv);
    free(A);
}

int main() {
    printf("--- Lancement du test de Eval ---\n\n");

    // On va construire l'arbre pour l'expression : 3 * x + 5

    // 1. Création des feuilles (les valeurs et la variable)
    typevaleur val_3 = {.reel = 3.0};
    Node* noeud_3 = creer_noeud(REEL, val_3, NULL, NULL);

    typevaleur val_5 = {.reel = 5.0};
    Node* noeud_5 = creer_noeud(REEL, val_5, NULL, NULL);

    typevaleur val_vide; // Pas de valeur spécifique pour une VARIABLE
    Node* noeud_x = creer_noeud(VARIABLE, val_vide, NULL, NULL);

    // 2. Création de la multiplication (3 * x)
    typevaleur val_fois = {.operateur = FOIS};
    Node* noeud_mult = creer_noeud(OPERATEUR, val_fois, noeud_3, noeud_x);


    typevaleur val_plus = {.operateur = PLUS};
    Node* racine = creer_noeud(OPERATEUR, val_plus, noeud_mult, noeud_5);


    // --- Testons notre arbre ! ---
    float x_test = 2.0;
    float resultat = Eval(racine, x_test);

    printf("Equation testee : 3 * x + 5\n");
    printf("Valeur de x     : %.2f\n", x_test);
    printf("Resultat obtenu : %.2f\n", resultat);
    printf("Resultat attendu: 11.00\n\n");

    if (resultat == 11.0f) {
        printf(">> Test REUSSI ! Ton eval.c fait le job.\n");
    } else {
        printf(">> Oups, y'a un loup. Test ECHOUE.\n");
    }

    liberer_arbre(racine);

    return 0;
}