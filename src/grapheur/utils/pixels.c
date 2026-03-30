#include "pixels.h"

/** Convert world coordinates to window pixel coordinates (rounded).
 * sx,sy may be NULL.
 */
void world_to_pixels(float wx, float wy, int *sx, int *sy) {
  if (g_win_w <= 0 || g_win_h <= 0) {
    if (sx)
      *sx = 0;
    if (sy)
      *sy = 0;
    return;
  }
  if (sx)
    *sx = (int)((wx - gx_min) * (float)g_win_w / (gx_max - gx_min) + 0.5f);
  if (sy)
    *sy = (int)((gy_max - wy) * (float)g_win_h / (gy_max - gy_min) + 0.5f);
}

/** Convert normalized device coords (-1..1) to pixel coordinates. */
void ndc_to_pixels(float nx, float ny, int *sx, int *sy) {
  float wx, wy;
  ndc_to_world(nx, ny, &wx, &wy);
  world_to_pixels(wx, wy, sx, sy);
}
