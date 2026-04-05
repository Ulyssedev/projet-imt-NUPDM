#ifndef GRAPHEUR_UTILS_CONST_H
#define GRAPHEUR_UTILS_CONST_H

/** Logical view bounds (world/cartesian coordinates) */
extern float gx_min;
extern float gx_max;
extern float gy_min;
extern float gy_max;

/** Window pixel dimensions (in pixels) */
extern int g_win_w;
extern int g_win_h;

/*save coords ping pointed with mouse*/
extern float saved_world_x;
extern float saved_world_y;
extern int show_coords;
extern int draw_coords;

#endif // GRAPHEUR_UTILS_CONST_H