
#ifndef GRAPHLIB_H
#define GRAPHLIB_H

#include <GL/glut.h>
#include <stdio.h>
#include <GL/freeglut.h>
#include "./utils/global.h"
#include "./utils/pixels.h"
#include "./utils/ndc.h"

/** Simple 2D point in world coordinates. */
typedef struct {
  float x;
  float y;
} Point;

/**
 * Initialize a GLUT window and OpenGL context.
 * @param argc pointer to program argc forwarded to GLUT
 * @param argv program argv forwarded to GLUT
 * @param w window width in pixels
 * @param h window height in pixels
 * @param title window title
 */
void graph_init_window(int *argc, char **argv, int w, int h, const char *title);

/** Set the background/clear color used by `glClear(GL_COLOR_BUFFER_BIT)`.
 * @param r red component (0..1)
 * @param g green component (0..1)
 * @param b blue component (0..1)
 */
void graph_set_background(float r, float g, float b);

/**
 * Apply the stored logical view to OpenGL (sets projection via glOrtho)
 * and reset the modelview matrix to identity. Call before issuing
 * vertex calls when you want coordinates mapped to the logical view.
 */
void graph_apply_view(void);

/**
 * Draw the X and Y axes across the current logical view.
 */
void graph_draw_axes(void);

/**
 * Draw a regular grid over the logical view.
 * @param x_step spacing between vertical grid lines
 * @param y_step spacing between horizontal grid lines
 */
void graph_draw_grid(float x_step, float y_step);

/**
 * Plot an array of points as a scatter plot.
 * @param pts array of Point
 * @param n number of points in pts
 * @param r,g,b color components (0..1)
 * @param size point size in pixels
 */
void graph_plot_points(const Point *pts, int n, float r, float g, float b,
                       float size);

/**
 * Draw a connected polyline through the given points.
 * @param pts array of Point
 * @param n number of points in pts
 * @param r,g,b color components (0..1)
 * @param width visual line thickness in pixels
 */
void graph_plot_lines(const Point *pts, int n, float r, float g, float b,
                      float width);

/**
 * Handle window resize: update viewport and internal pixel size tracking.
 * Can be passed directly to `glutReshapeFunc`.
 */
void graph_reshape(int w, int h);

/**
 * Draw a bitmap text string at window coordinates (pixels, origin at bottom-left).
 * @param text null-terminated string to draw
 * @param x window x in pixels (from left)
 * @param y window y in pixels (from bottom)
 */
void graph_draw_text(const char *text, int x, int y);

/**
 * Convenience: draw text anchored near the top-left of the window with
 * a small padding.
 * @param text null-terminated string to draw
 */
void graph_draw_text_top_left(const char *text);

/*Draw small lines to graduate axis*/
void graph_draw_grid_min_lines(float x_step, float y_step);

void graph_draw_numbers(float x_step, float y_step);

/*Indicator coordonnes top right*/
void graph_draw_coords_top_right(float x, float y);

/*Draw red lines from axis to point coords*/
void graph_draw_coords_red_lines(float x, float y);


#endif // GRAPHLIB_H
