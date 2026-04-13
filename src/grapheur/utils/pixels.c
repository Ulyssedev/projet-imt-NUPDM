#include "pixels.h"

/** Convertit des coordonnées world en coordonnées pixel (arrondies).
 * wx, wy : coordonnées world ; sx, sy peuvent être NULL si le demandeur
 * ne veut que l'une des deux composantes.
 */
void world_to_pixels(float wx, float wy, int *sx, int *sy) {
  if (g_win_w <= 0 || g_win_h <= 0) {
    if (sx)
      *sx = 0;
    if (sy)
      *sy = 0;
    return;
  }
  /* conversion X : proportion relative dans l'intervalle world -> pixels */
  if (sx)
    *sx = (int)((wx - gx_min) * (float)g_win_w / (gx_max - gx_min) + 0.5f);
  /*
   * conversion Y : on mappe la coordonnée world vers la hauteur en pixels
   * puis on arrondit. (Remarque : l'implémentation actuelle calcule sy en
   * fonction de gy_min -> gy_max.)
   */
  if (sy)
    *sy = (int)((wy - gy_min) * (float)g_win_h / (gy_max - gy_min) + 0.5f);
}

/** Convertit des coordonnées NDC (-1..1) en coordonnées pixel. */
void ndc_to_pixels(float nx, float ny, int *sx, int *sy) {
  float wx, wy;
  ndc_to_world(nx, ny, &wx, &wy);
  world_to_pixels(wx, wy, sx, sy);
}
