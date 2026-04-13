#include "world.h"

/** Convertit une coordonnée pixel (fenêtre) en coordonnées world.
 * sx, sy : coordonnées pixel (0,0 = coin gauche, haut). wx, wy peuvent
 * être NULL si l'appelant ne souhaite que l'une des deux valeurs.
 */
void pixels_to_world(int sx, int sy, float *wx, float *wy) {
  if (g_win_w <= 0 || g_win_h <= 0) {
    if (wx)
      *wx = 0.0f;
    if (wy)
      *wy = 0.0f;
    return;
  }
  if (wx)
    *wx = gx_min + (float)sx * (gx_max - gx_min) / (float)g_win_w;
  /*
   * Attention : l'origine des pixels est en haut (sy = 0 en haut de la
   * fenêtre), donc on mappe sy vers gy_max moins la fraction verticale.
   */
  if (wy)
    *wy = gy_max - (float)sy * (gy_max - gy_min) / (float)g_win_h;
}

/** Convertit des coordonnées NDC (-1..1) en coordonnées world.
 * nx, ny : coordonnées NDC (-1..1). wx, wy peuvent être NULL.
 */
void ndc_to_world(float nx, float ny, float *wx, float *wy) {
  if (wx)
    *wx = ((nx + 1.0f) * 0.5f) * (gx_max - gx_min) + gx_min;
  if (wy)
    *wy = ((ny + 1.0f) * 0.5f) * (gy_max - gy_min) + gy_min;
}

/** Met à jour les limites du monde utilisées pour la projection et les
 * conversions.
 */
void world_set_view(float x_min, float x_max, float y_min, float y_max) {
  gx_min = x_min;
  gx_max = x_max;
  gy_min = y_min;
  gy_max = y_max;
}

/** Met à jour la taille de fenêtre mise en cache pour les conversions. */
void world_set_window_size(int w, int h) {
  g_win_w = (w > 0) ? w : 1;
  g_win_h = (h > 0) ? h : 1;
}

/** Zoom centré sur (center_x, center_y) par le facteur `factor`.
 * La mise à l'échelle préserve la position relative du point focal afin
 * que le point sous le curseur conserve sa position en pixels après zoom.
 */
void world_zoom_at(float center_x, float center_y, float factor) {
  if (factor <= 0.0f)
    return;
  float old_w = gx_max - gx_min;
  float old_h = gy_max - gy_min;
  if (old_w <= 0.0f || old_h <= 0.0f) {
    /* Si les limites ne sont pas valides, on fait un zoom centré simple */
    float new_w = old_w / factor;
    float new_h = old_h / factor;
    gx_min = center_x - new_w * 0.5f;
    gx_max = center_x + new_w * 0.5f;
    gy_min = center_y - new_h * 0.5f;
    gy_max = center_y + new_h * 0.5f;
    return;
  }

  float new_w = old_w / factor;
  float new_h = old_h / factor;

  /*
   * Calculer la fraction horizontale (0..1) de center_x dans l'ancienne
   * vue pour préserver sa position relative après changement d'échelle.
   */
  float ux = (center_x - gx_min) / old_w; /* 0..1 depuis la gauche */
  float new_gx_min = center_x - ux * new_w;
  float new_gx_max = new_gx_min + new_w;

  /*
   * Pour Y on mesure la fraction depuis le haut car l'origine des pixels
   * est en haut de la fenêtre (sy=0 en haut).
   */
  float uy = (gy_max - center_y) / old_h; /* 0..1 depuis le haut */
  float new_gy_max = center_y + uy * new_h;
  float new_gy_min = new_gy_max - new_h;

  gx_min = new_gx_min;
  gx_max = new_gx_max;
  gy_min = new_gy_min;
  gy_max = new_gy_max;
}

/** Déplacement (pan) du monde de dx,dy (en unités world). */
void world_pan(float dx, float dy) {
  gx_min += dx;
  gx_max += dx;
  gy_min += dy;
  gy_max += dy;
}
