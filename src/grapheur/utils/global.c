/*
 * Global state for the grapheur "world" coordinate system.
 * These variables represent the currently visible logical bounds
 * and the window pixel dimensions used by conversion helpers.
 */
float gx_min = -10.0f, gx_max = 10.0f, gy_min = -10.0f, gy_max = 10.0f;

/* Window pixel dimensions (width, height). */
int g_win_w = 1, g_win_h = 1;


/*save coords ping pointed with mouse*/
float saved_world_x = 0.0f;
float saved_world_y = 0.0f;
int show_coords = 0; // Indicator : 0 = hide, 1 = show
int draw_coords = 0; // Indicator : 0 = hide, 1 = show