#ifndef GRAPHEUR_UTILS_WORLD_H
#define GRAPHEUR_UTILS_WORLD_H

#include "global.h"

/** Convertit des coordonnées pixel (fenêtre) en coordonnées world/cartésiennes.
 * @param sx abscisse pixel (0 = gauche)
 * @param sy ordonnée pixel (0 = haut)
 * @param wx sortie abscisse world (peut être NULL)
 * @param wy sortie ordonnée world (peut être NULL)
 */
void pixels_to_world(int sx, int sy, float *wx, float *wy);

/** Convertit des coordonnées NDC (-1..1) en coordonnées world.
 * @param nx coordonnée NDC X (-1..1)
 * @param ny coordonnée NDC Y (-1..1)
 * @param wx sortie abscisse world (peut être NULL)
 * @param wy sortie ordonnée world (peut être NULL)
 */
void ndc_to_world(float nx, float ny, float *wx, float *wy);

/* Gestion de la vue logique et de la taille de fenêtre */

/**
 * Définit les bornes logiques utilisées pour la projection orthographique.
 * Ces bornes sont prises en compte par `graph_apply_view()`.
 * @param x_min bord gauche
 * @param x_max bord droit
 * @param y_min bord inférieur
 * @param y_max bord supérieur
 */
void world_set_view(float x_min, float x_max, float y_min, float y_max);

/**
 * Met à jour la taille de la fenêtre utilisée par les helpers de conversion.
 * Les valeurs <= 0 sont replacées à 1.
 * @param w largeur en pixels
 * @param h hauteur en pixels
 */
void world_set_window_size(int w, int h);

/**
 * Zoom centré sur la coordonnée world donnée.
 * factor > 1.0 -> zoom avant, factor < 1.0 -> zoom arrière.
 * @param center_x abscisse world à conserver fixe
 * @param center_y ordonnée world à conserver fixe
 * @param factor facteur de zoom
 */
void world_zoom_at(float center_x, float center_y, float factor);

/**
 * Décale la vue de dx, dy (en unités world).
 * @param dx translation en X (unités world)
 * @param dy translation en Y (unités world)
 */
void world_pan(float dx, float dy);

#endif // GRAPHEUR_UTILS_WORLD_H
