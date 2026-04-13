# Grapheur — documentation du module

Ce module fournit l'interface graphique de l'application : initialisation
de la fenêtre OpenGL/GLUT, gestion de la vue logique (coordonnées "world"),
rendu 2D (axes, grille, graduations, texte) et traçage de fonctions
mathématiques évaluées via le module `evaluateur`.

Le grapheur permet aussi d'ajouter/éditer/supprimer des fonctions via une
surcouche UI (menu) et prend en charge les interactions usuelles : panoramique
(drag), zoom (molette), sélection au clavier et affichage de coordonnées.

Depuis les dernières évolutions, le menu colore aussi les indices de la liste
des fonctions avec une palette commune, et la touche `b` verrouille le domaine
de tracé des courbes sur les bornes X courantes. Quand ce verrou est actif, les
fonctions sont échantillonnées sur l'intervalle figé même si la vue se déplace
ou change de taille.

## Fichiers principaux

- `graphlib.c` / `graphlib.h` : initialisation de la fenêtre, application de la
	vue orthographique (`graph_apply_view`), fonctions utilitaires de dessin
	(axes, grille, graduations, texte, tracé de points et de lignes). Gère
	aussi la conversion du rendu entre coordonnées "world" et pixels pour
	l'affichage d'UI.
- `menu.c` / `menu.h` : surcouche visuelle (overlay) gérant le stockage des
	fonctions utilisateur (`funcs`), le cache d'arbres syntaxiques (`func_trees`)
	via l'API `pipeline_build_arbre`/`pipeline_free_arbre`, et les fonctions
	d'ajout/édition/suppression. Dessine le menu et le bouton de bascule. Les
	indices de la liste utilisent la palette partagée définie dans
	`src/common/graph_config.c`.
- `entry.c` / `entry.h` : callbacks d'entrée (clavier, souris, touches
	spéciales). Délègue les événements au menu quand nécessaire et gère
	le pan/zoom ainsi que les raccourcis locaux.
- `utils/` : petits utilitaires pour l'état global et les conversions :
	- `global.c` / `global.h` : variables globales partagées (`gx_min`,
		`gx_max`, `g_win_w`, `g_win_h`, flags d'affichage, coordonnées
		sauvegardées...).
	- `world.c` / `world.h` : conversions pixels <-> world, gestion de la vue
		(`world_set_view`, `world_set_window_size`, `world_zoom_at`, `world_pan`).
	- `pixels.c` / `pixels.h` et `ndc.c` / `ndc.h` : conversions entre
		coordonnées pixel, NDC (-1..1) et coordonnées world.

## Intégration et flux d'exécution

- Le point d'entrée principal (`main.c`) :
	- initialise la fenêtre avec `graph_init_window`, définit la couleur de
		fond (`graph_set_background`) et la vue logique (`world_set_view`).
	- initialise le menu (`menu_init`) et les handlers d'entrée (`entry_init`).
	- enregistre les callbacks GLUT : `display`, `graph_reshape`, `idle_cb`.
- Callback `display` (boucle de rendu) :
	- dessine la grille, les graduations, les nombres et les axes via
		`graph_draw_*`.
	- si aucune fonction n'est définie : dessine une sinusoïde par défaut.
	- sinon, pour chaque fonction stockée, récupère l'arbre (AST) via
		`menu_get_cached_arbre(idx)` et, si disponible, échantillonne la fonction
		en appelant `Eval(root, x)` (module `evaluateur`) pour construire un
		tableau de `Point` puis trace la courbe avec `graph_plot_lines`.
	- dessine le bouton/menu visuel (`menu_draw_button`, `menu_draw_overlay`).

## Interactions utilisateur

- Clavier (gestion dans `entry.c` et `menu.c`) :
	- `m` / `M` : basculer la visibilité du menu.
	- `a` : commencer l'ajout d'une nouvelle fonction (entrée en mode saisie).
	- `e` : éditer la fonction sélectionnée.
	- `d` : supprimer la fonction sélectionnée.
	- `q` : masquer le menu.
	- `Entrée` : valider la saisie (ajout/édition).
	- `Échap` : annuler l'édition.
	- `Backspace` : effacer un caractère de la saisie.
	- Flèches Haut/Bas : naviguer entre les fonctions dans le menu.
	- `espace` : sauvegarder les coordonnées du curseur et les afficher.
	- `t` : basculer l'affichage des lignes rouges reliant les axes au point
		sauvegardé.
	- `b` : verrouiller/déverrouiller le domaine X utilisé pour le tracé des
		courbes.
- Souris :
	- Clic gauche + drag : panoramique (pan) de la vue.
	- Molette (boutons 3/4 dans GLUT) : zoom centré sur la position du
		curseur.
	- Clic droit : activer/désactiver le suivi des coordonnées sous le curseur.
	- Clic sur le petit bouton en haut-gauche : ouvrir/fermer le menu.

## Détails d'implémentation importants

- Échantillonnage : la densité d'échantillonnage est contrôlée par la
	constante `NPOINTS` (définie dans `src/common/graph_config.h`, valeur
	par défaut : 10000). Réduire `NPOINTS` améliore les performances sur
	machines lentes ; l'augmenter lisse le tracé.
- Tracé des courbes : `graph_plot_lines` gère correctement les points non
	finis (NaN/Inf) et sépare les segments quand un saut vertical important
	est détecté (empêche de tracer de longues lignes entre deux régions
	discontinues, p.ex. asymptotes).
- Graduations intelligentes : `choose_step` et `compute_nice_step` calculent
	des pas « agréables » (série 1,2,5 × 10^k) pour les ticks et adaptent le
	pas en fonction du zoom pour garder la grille lisible.
- Cache d'ASTs : le menu tente de parser l'expression lors de l'ajout/édition
	(`pipeline_build_arbre`) et stocke l'arbre dans `func_trees` pour éviter de
	reparser à chaque frame. En cas d'échec de parsing, la chaîne reste
	disponible pour édition.
- Insertion des nouvelles fonctions : l'ajout se fait à la position courante
	de la sélection dans le menu, puis la nouvelle entrée devient sélectionnée.
- Palette partagée : les couleurs utilisées par le menu et le grapheur sont
	définies une seule fois dans `src/common/graph_config.c` pour éviter les
	définitions multiples.
