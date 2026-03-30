#ifndef GRAPHEUR_UTILS_WORLD_H
#define GRAPHEUR_UTILS_WORLD_H

#include "global.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Convert from pixel (window) coordinates to cartesian world coordinates.
 * @param sx x pixel (0 = left)
 * @param sy y pixel (0 = top)
 * @param wx out world x (may be NULL)
 * @param wy out world y (may be NULL)
 */
void pixels_to_world(int sx, int sy, float *wx, float *wy);

/** Convert from NDC (-1..1) coordinates to cartesian world coordinates.
 * @param nx normalized device x (-1..1)
 * @param ny normalized device y (-1..1)
 * @param wx out world x (may be NULL)
 * @param wy out world y (may be NULL)
 */
void ndc_to_world(float nx, float ny, float *wx, float *wy);

/* Manage the logical world/view and window size. */

/**
 * Set the logical rendering bounds for the orthographic projection.
 * These bounds are applied when `graph_apply_view()` is called.
 * @param x_min left bound
 * @param x_max right bound
 * @param y_min bottom bound
 * @param y_max top bound
 */
void world_set_view(float x_min, float x_max, float y_min, float y_max);

/**
 * Set current window pixel dimensions used by conversion helpers.
 * Values <= 0 are clamped to 1.
 * @param w width in pixels
 * @param h height in pixels
 */
void world_set_window_size(int w, int h);

/**
 * Zoom the view around the given world coordinate by a factor.
 * factor > 1.0 -> zoom in, factor < 1.0 -> zoom out.
 * @param center_x world x coordinate to keep fixed
 * @param center_y world y coordinate to keep fixed
 * @param factor zoom factor
 */
void world_zoom_at(float center_x, float center_y, float factor);

/**
 * Pan the view by world-space offsets.
 * @param dx shift in x (world units)
 * @param dy shift in y (world units)
 */
void world_pan(float dx, float dy);

#ifdef __cplusplus
}
#endif

#endif // GRAPHEUR_UTILS_WORLD_H
