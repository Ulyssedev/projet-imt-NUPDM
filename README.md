# Projet IMT - Techniques de programmation avancées
Ce projet à pour but de faire un interpréteur graphique en C pour la matière programmation avancée en C de l'IMT.

## Équipe de développement

### Chef de projet
Ulysse Curier

### Lexique
Amine Mallem \
Titouan Genoud \
Karl Bouisseren

### Syntaxe
Ulysse Curier \
Kaufman Adam

### Evaluateur
Adrien Vancompernolle \
Wassim Ouffroukh

### Grapheur
Elie Haidar \
Alexandre Teyssier

## Structure du projet proposée

```
├── ReadMe.MD           # Rapports d'utilisation, maintenance et ReadMe (à séparer dans d'autres fichiers potentiellement)
├── include/            # Fichiers d'interface (.h) communs à tous les modules 
├── src/                # Code source (.c) réparti par binôme
│   ├── dialogueur/     # Gestion de l'IHM et saisie utilisateur
│   ├── lexical/        # Analyseur lexical (lexèmes/jetons)
│   ├── syntaxique/     # Analyseur syntaxique et génération de code
│   ├── evaluateur/     # Évaluateur et gestion de la pile 
│   └── common/         # Structures de données partagées (ex: Couple lexème-valeur)
├── lib/                # Bibliothèques externes (graph.h, graph.c, OpenGL) 
├── tests/              # Programmes principaux de test pour chaque module (si possible)
└── Makefile            # Automatisation de la compilation et édition des liens
```

*Cette structure est à titre indicatif et va surement évoluer au cours du projet*
