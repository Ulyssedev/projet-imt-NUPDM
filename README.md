# Projet IMT Nord Europe - Interpréteur graphique en C

Ce dépôt contient le projet de programmation avancée de l'IMT Nord Europe, réaliser un interpréteur graphique en C, réparti en plusieurs sous-modules (lexical, syntaxique, évaluateur, dialogueur et grapheur).


## Équipe

- **Chef de projet** : Ulysse Curier
- **Lexique** : Amine Mallem, Titouan Genoud, Karl Bouisseren
- **Syntaxe** : Ulysse Curier, Kaufman Adam
- **Évaluateur** : Adrien Vancompernolle, Wassim Ouffroukh
- **Grapheur** : Elie Haidar, Alexandre Teyssier

## Structure du dépôt

Arborescence principale :

```
├── ReadMe.MD           # Rapports d'utilisation, maintenance et ReadMe (à séparer dans d'autres fichiers potentiellement)
├── include/            # Fichiers d'interface (.h) communs à tous les modules 
├── src/                # Code source (.c) réparti par binôme
│   ├── dialogueur/     # Gestion de l'IHM et saisie utilisateur
│   ├── lexical/        # Analyseur lexical (lexèmes/jetons)
│   ├── syntaxique/     # Analyseur syntaxique et génération de code
│   ├── evaluateur/     # Évaluateur et gestion de la pile 
│   └── common/         # Structures de données partagées (ex: Couple lexème-valeur)
├── tests/              # Programmes principaux de test pour chaque module (si possible)
└── Makefile            # Automatisation de la compilation et édition des liens
```

## Documentations par module

- [Syntaxique](src/syntaxique/README.md)
- [Evaluateur](src/evaluateur/README.md)
- [Grapheur](src/grapheur/README.md)
- [Lexique](src/lexical/README.md)

## Binaires précompilés

Des binaires précompilés pour Windows et Linux sont disponibles dans la section [Releases](https://github.com/Ulyssedev/projet-imt-NUPDM/releases/latest) du Github.

## Prérequis

Installer les dépendances nécessaires pour la compilation et l'exécution (Ubuntu) :

```bash
sudo apt update
sudo apt install build-essential libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev
```

Le Makefile utilise `gcc` par défaut. Vous pouvez remplacer le compilateur et les flags en ligne de commande :

```bash
make CC=clang CFLAGS="-O2"
```

## Compilation

Les exécutables sont placés dans le répertoire `build/`.

- **Compilation complète (par défaut)** :

```bash
make        # équivalent à `make all`
# Sortie : build/projet-imt
```

- **Construire un sous-module spécifique** :

Le `Makefile` définit des cibles par sous-groupe et chaque cible produit un binaire distinct dans `build/` :

- **Grapheur** :

```bash
make grapheur
# Sortie : build/projet-imt-grapheur
```

- **Lexical** :

```bash
make lexical
# Sortie : build/projet-imt-lexical
```

- **Dialogueur** :

```bash
make dialogueur
# Sortie : build/projet-imt-dialogueur
```

- **Evaluateur** :

```bash
make evaluateur
# Sortie : build/projet-imt-evaluateur
```

- **Syntaxique** :

```bash
make syntaxique
# Sortie : build/projet-imt-syntaxique
```

- **Application finale chainee** :

```bash
make project
# Sortie : build/projet-imt-project
```

- **Lancer les tests** :

```bash
make test
# Exécute : build/test-syntaxique et build/test-evaluateur
```

Cette cible compile le chainage utilisateur complet utile pour la demonstration:
analyse lexicale -> analyse syntaxique -> generation/lecture du code postfixe ->
evaluation -> affichage grapheur.

Notes :

- Les cibles compilent les sources du sous-dossier concerné + `src/common`.
- Les objets temporaires sont générés sous `build/` en conservant l'arborescence source.

## Exécution

Après compilation, lancez l'exécutable correspondant depuis la racine du dépôt, par exemple :

```bash
./build/projet-imt-grapheur
# ou
./build/projet-imt
```

Pour l'application finale chainee :

```bash
./build/projet-imt-project
```

Au lancement sans argument, le programme demande interactivement:
- l'expression,
- la borne inferieure,
- la borne superieure,
puis ouvre la fenetre grapheur avec la fonction demandee.

Lancement non interactif possible (utile pour tests):

```bash
./build/projet-imt-project "sin(x)" -10 10
```

## Nettoyage

Pour supprimer les fichiers de build :

```bash
make clean
```
