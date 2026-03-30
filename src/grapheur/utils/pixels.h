#ifndef GRAPHEUR_UTILS_PIXELS_H
#define GRAPHEUR_UTILS_PIXELS_H

#include "world.h"
#include "global.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Convert from world/cartesian coordinates to pixel (window) coordinates.
 * @param wx world x
 * @param wy world y
 * @param sx out x pixel (may be NULL)
 * @param sy out y pixel (may be NULL)
 */
void world_to_pixels(float wx, float wy, int *sx, int *sy);

/** Convert from NDC (-1..1) to pixel coordinates. */
void ndc_to_pixels(float nx, float ny, int *sx, int *sy);

#ifdef __cplusplus
}
#endif

#endif // GRAPHEUR_UTILS_PIXELS_H
