#ifndef GRAPHEUR_UTILS_NDC_H
#define GRAPHEUR_UTILS_NDC_H

#include "global.h"
#include "world.h"

/** Convertit des coordonnées pixel (fenêtre) en NDC (-1..1).
 * @param sx abscisse pixel (0 = gauche)
 * @param sy ordonnée pixel (0 = haut)
 * @param nx sortie coordonnée normalisée X (-1..1)
 * @param ny sortie coordonnée normalisée Y (-1..1)
 */
void pixels_to_ndc(int sx, int sy, float *nx, float *ny);

/** Convertit des coordonnées world/cartésiennes en NDC (-1..1).
 * @param wx abscisse world
 * @param wy ordonnée world
 * @param nx sortie coordonnée normalisée X (-1..1)
 * @param ny sortie coordonnée normalisée Y (-1..1)
 */
void world_to_ndc(float wx, float wy, float *nx, float *ny);

#endif // GRAPHEUR_UTILS_NDC_H
