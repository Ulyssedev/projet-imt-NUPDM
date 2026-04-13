#ifndef GRAPHEUR_UTILS_CONST_H
#define GRAPHEUR_UTILS_CONST_H

/** Bornes de la vue logique (coordonnées world/cartésiennes) */
extern float gx_min;
extern float gx_max;
extern float gy_min;
extern float gy_max;

/** Bornes X utilisées pour le tracé des courbes quand elles sont verrouillées.
 */
extern float trace_gx_min;
extern float trace_gx_max;
extern int trace_x_locked;

/** Dimensions de la fenêtre en pixels (largeur, hauteur) */
extern int g_win_w;
extern int g_win_h;

/* Coordonnées sauvegardées (point sous le curseur) */
extern float saved_world_x;
extern float saved_world_y;

/* Flags d'affichage des coordonnées : 0 = caché, 1 = affiché */
extern int show_coords;
extern int draw_coords;

#endif // GRAPHEUR_UTILS_CONST_H