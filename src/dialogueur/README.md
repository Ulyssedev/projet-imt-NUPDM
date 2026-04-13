# Module Dialogueur

## Objectif du module

Le dialogueur assure la passerelle entre la saisie textuelle et l'exécution de
l'expression mathématique. Il prend en charge le parcours du pipeline complet :
analyse lexicale, analyse syntaxique, construction de l'arbre, puis évaluation
ponctuelle d'une expression pour une valeur de `x` donnée.

Ce module est utilisé à deux endroits principaux :

- pour construire un arbre réutilisable quand une fonction doit être stockée
  dans le grapheur,
- pour calculer directement une valeur numérique via une chaîne d'expression,
  avec un message d'erreur lisible en cas d'échec.

## Fichiers du module

- `pipeline.h` : interface publique du pipeline.
- `pipeline.c` : implémentation du chaînage lexical -> syntaxique -> arbre ->
  évaluation.

## Fonctions principales

- `pipeline_build_arbre` : convertit une expression textuelle en arbre syntaxique
  exploitable par le module évaluateur.
- `calculer_fx` : calcule directement `f(x)` à partir d'une chaîne de
  caractères, en renvoyant un code d'état et, si besoin, un message d'erreur.
- `liberer_arbre_pipeline` : libère récursivement un arbre créé par le pipeline.

## Comportement du pipeline

Le pipeline suit les étapes suivantes :

1. le texte est analysé par le lexer,
2. les jetons sont convertis en notation postfixée,
3. la postfixe est transformée en arbre binaire,
4. l'évaluateur calcule le résultat ou remonte une erreur mathématique.

Les erreurs lexicales, syntaxiques, mémoire et évaluation sont distinguées afin
de pouvoir afficher un message précis dans l'interface graphique.

## Notes d'utilisation

- Une expression invalide ne doit jamais être évaluée directement : il faut
  passer par `pipeline_build_arbre` ou `calculer_fx`.
- Les arbres renvoyés par `pipeline_build_arbre` appartiennent à l'appelant.
  Ils doivent être libérés avec `liberer_arbre_pipeline`.
- `calculer_fx` libère automatiquement l'arbre temporaire qu'il crée.

## Intégration

Le grapheur s'appuie sur ce module pour parser les fonctions saisies par
l'utilisateur avant de les stocker dans son cache. Cela permet d'afficher les
erreurs au moment de l'ajout ou de l'édition, sans reparser à chaque frame.