# Module Syntaxique

## Objectif du module

Le module syntaxique recoit une sequence de `typejeton` produite par l'analyse lexicale,
verifie la conformite grammaticale, puis produit une representation exploitable par
l'evaluateur :

- une forme postfixee (`convertir_en_postfixe`),
- puis un arbre binaire (`convertir_code_postfixe_en_arbre`).

Ce module respecte la grammaire du sujet :

- `exp ::= REEL`
- `exp ::= VARIABLE`
- `exp ::= FONCTION exp`
- `exp ::= ( exp OPERATEUR exp )`
- `exp ::= ( exp )`
- `expression_complete ::= exp FIN`

## Choix de conception

### 1) Conversion vers postfixe

La conversion est implementee avec un parseur recursif descendant (fonction interne
`analyser_exp`).

Raisonnement :

- la grammaire est simple et bien adaptee a une descente recursive,
- chaque regle produit naturellement son equivalent postfixe,
- le code reste court, lisible et proche du cahier des charges.

Principe :

- une feuille (`REEL`, `VARIABLE`) est ecrite directement en sortie,
- une fonction unaire ecrit d'abord son argument, puis le jeton fonction,
- une operation binaire parenthesee ecrit `gauche droite operateur`,
- la fonction verifie explicitement la presence du `FIN` en entree,
- en cas de succes, un jeton `FIN` est aussi pose en sortie postfixee.

### 2) Conversion postfixe vers arbre

La conversion est realisee par pile de noeuds (technique standard postfixe -> AST).

Raisonnement :

- la notation postfixee se traite naturellement avec une pile,
- complexite lineaire `O(n)`,
- implementation robuste pour les erreurs de forme.

Principe :

- `REEL` / `VARIABLE` : creation d'un noeud feuille et empilement,
- `FONCTION` : depilement d'un operande, creation d'un noeud unaire, empilement,
- `OPERATEUR` : depilement droite + gauche, creation d'un noeud binaire, empilement,
- fin valide si la pile contient exactement une racine.

## Gestion des erreurs

Les codes d'erreur utilises sont definis dans `syntaxique.h` :

- `SYNTAXE_OK`
- `SYNTAXE_ERREUR_ARGUMENT`
- `SYNTAXE_ERREUR_GRAMMAIRE`
- `SYNTAXE_ERREUR_FIN_MANQUANTE`

Le module detecte notamment :

- pointeurs d'entree/sortie invalides,
- expression ne respectant pas la grammaire,
- postfixe invalide (manque d'operandes / pile incoherente),
- absence de jeton `FIN` attendu.

En cas d'echec pendant la construction d'arbre, les noeuds alloues sont liberes.

## Fichiers du module

- `syntaxique.h` : interface publique du module,
- `main.c` : implementation des deux conversions.

## Tests

Les tests syntaxiques sont centralises dans `tests/test_syntaxique.c`.

Ils couvrent :

- expression binaire valide,
- fonction unaire valide,
- erreur de fin manquante,
- erreur grammaticale,
- conversion postfixe vers arbre (verification de structure).

Execution :

```bash
make test
```
