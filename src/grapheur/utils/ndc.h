#ifndef GRAPHEUR_UTILS_NDC_H
#define GRAPHEUR_UTILS_NDC_H

#include "world.h"
#include "global.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Convert from window pixel coordinates to NDC (-1..1).
 * @param sx x pixel (0 = left)
 * @param sy y pixel (0 = top)
 * @param nx out normalized x (-1..1)
 * @param ny out normalized y (-1..1)
 */
void pixels_to_ndc(int sx, int sy, float *nx, float *ny);

/** Convert from world/cartesian coordinates to NDC (-1..1).
 * @param wx world x
 * @param wy world y
 * @param nx out normalized x (-1..1)
 * @param ny out normalized y (-1..1)
 */
void world_to_ndc(float wx, float wy, float *nx, float *ny);

#ifdef __cplusplus
}
#endif

#endif // GRAPHEUR_UTILS_NDC_H
