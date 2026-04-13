/*
 * État global pour le système de coordonnées du grapheur ("world").
 * Ces variables représentent les limites logiques visibles actuellement
 * ainsi que la taille de la fenêtre en pixels utilisée par les helpers
 * de conversion (pixels <-> world).
 */
float gx_min = -10.0f, gx_max = 10.0f, gy_min = -10.0f, gy_max = 10.0f;

/* Taille de la fenêtre en pixels (largeur, hauteur). */
int g_win_w = 1, g_win_h = 1;

/* Coordonnées sauvegardées (pointées par la souris) */
float saved_world_x = 0.0f;
float saved_world_y = 0.0f;

/* Flags d'affichage des coordonnées : 0 = caché, 1 = affiché */
int show_coords = 0;
int draw_coords = 0;