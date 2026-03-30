#include "world.h"

/** Convert a window pixel coordinate to world coordinates.
 * sx,sy: pixel coordinate (0,0 = left,top). wx,wy may be NULL.
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
  if (wy)
    *wy = gy_max - (float)sy * (gy_max - gy_min) / (float)g_win_h;
}

/** Convert normalized device coords (-1..1) to world coords.
 * nx,ny: NDC coords. wx,wy may be NULL.
 */
void ndc_to_world(float nx, float ny, float *wx, float *wy) {
  if (wx)
    *wx = ((nx + 1.0f) * 0.5f) * (gx_max - gx_min) + gx_min;
  if (wy)
    *wy = ((ny + 1.0f) * 0.5f) * (gy_max - gy_min) + gy_min;
}

/** Update the world bounds used for projection and conversions. */
void world_set_view(float x_min, float x_max, float y_min, float y_max) {
  gx_min = x_min;
  gx_max = x_max;
  gy_min = y_min;
  gy_max = y_max;
}

/** Update the cached window pixel size for coordinate conversions. */
void world_set_window_size(int w, int h) {
  g_win_w = (w > 0) ? w : 1;
  g_win_h = (h > 0) ? h : 1;
}

/** Zoom the world around center_x,center_y by factor. */
void world_zoom_at(float center_x, float center_y, float factor) {
  if (factor <= 0.0f)
    return;
  float wx = gx_max - gx_min;
  float hy = gy_max - gy_min;
  float new_w = wx / factor;
  float new_h = hy / factor;
  gx_min = center_x - new_w * 0.5f;
  gx_max = center_x + new_w * 0.5f;
  gy_min = center_y - new_h * 0.5f;
  gy_max = center_y + new_h * 0.5f;
}

/** Pan the world by dx,dy (in world units). */
void world_pan(float dx, float dy) {
  gx_min += dx;
  gx_max += dx;
  gy_min += dy;
  gy_max += dy;
}
