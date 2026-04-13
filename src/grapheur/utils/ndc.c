#include "ndc.h"

/** Convertit des coordonnées pixel (fenêtre) en NDC (-1..1). */
void pixels_to_ndc(int sx, int sy, float *nx, float *ny) {
  float wx, wy;
  pixels_to_world(sx, sy, &wx, &wy);
  world_to_ndc(wx, wy, nx, ny);
}

/** Convertit des coordonnées world en normalized device coordinates (-1..1).
 * La conversion est linéaire sur chaque axe, en prenant en compte les bornes
 * `gx_min/gx_max` et `gy_min/gy_max`.
 */
void world_to_ndc(float wx, float wy, float *nx, float *ny) {
  if (gx_max == gx_min || gy_max == gy_min) {
    if (nx)
      *nx = 0.0f;
    if (ny)
      *ny = 0.0f;
    return;
  }
  if (nx)
    *nx = ((wx - gx_min) / (gx_max - gx_min)) * 2.0f - 1.0f;
  if (ny)
    *ny = ((wy - gy_min) / (gy_max - gy_min)) * 2.0f - 1.0f;
}
