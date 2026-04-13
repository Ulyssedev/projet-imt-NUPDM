#ifndef GRAPHEUR_UTILS_PIXELS_H
#define GRAPHEUR_UTILS_PIXELS_H

#include "global.h"
#include "world.h"

/** Convertit des coordonnées world/cartésiennes en coordonnées pixel (fenêtre).
 * @param wx abscisse world
 * @param wy ordonnée world
 * @param sx sortie abscisse en pixels (peut être NULL)
 * @param sy sortie ordonnée en pixels (peut être NULL)
 */
void world_to_pixels(float wx, float wy, int *sx, int *sy);

/** Convertit des coordonnées NDC (-1..1) en coordonnées pixel. */
void ndc_to_pixels(float nx, float ny, int *sx, int *sy);

#endif // GRAPHEUR_UTILS_PIXELS_H
