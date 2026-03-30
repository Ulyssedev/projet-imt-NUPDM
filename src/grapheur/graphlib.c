#include "graphlib.h"
#include "utils/world.h"
#include <math.h>

PFNGLWINDOWPOS2IPROC glWindowPos2i;

static float bg_r = 0.0f, bg_g = 0.0f, bg_b = 0.0f;

/** Initialize a GLUT window and OpenGL context.
 * @param argc pointer to program argc forwarded to GLUT
 * @param argv program argv forwarded to GLUT
 * @param w window width in pixels
 * @param h window height in pixels
 * @param title window title
 */
void graph_init_window(int *argc, char **argv, int w, int h,
                       const char *title) {
  glutInit(argc, argv);
  /* Request a single front buffer and RGB color mode. */
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(w, h);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(title);
  /* Remember initial window size and set viewport. */
  world_set_window_size(w, h);
  glViewport(0, 0, (w > 0) ? w : 1, (h > 0) ? h : 1);
  glClearColor(bg_r, bg_g, bg_b, 1.0f);
}

/** Set the background/clear color used by the renderer.
 * Also updates the GL clear color state.
 * @param r red component (0..1)
 * @param g green component (0..1)
 * @param b blue component (0..1)
 */
void graph_set_background(float r, float g, float b) {
  bg_r = r;
  bg_g = g;
  bg_b = b;
  glClearColor(bg_r, bg_g, bg_b, 1.0f);
}

/** Apply the current logical view to OpenGL.
 * Sets an orthographic projection matching the world bounds and
 * resets the modelview matrix to identity.
 */
void graph_apply_view(void) {
  /* Switch to the projection matrix (camera / projection settings). */
  glMatrixMode(GL_PROJECTION);
  /* Reset projection matrix to identity to avoid accumulating transforms. */
  glLoadIdentity();
  /* Define an orthographic projection matching our logical bounds.
    left = gx_min, right = gx_max,
    bottom = gy_min, top = gy_max,
    near = -1, far = 1 (sufficient for 2D rendering). */
  glOrtho(gx_min, gx_max, gy_min, gy_max, -1.0f, 1.0f);
  /* Switch back to modelview matrix for object transforms and reset it. */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/** Draw the X and Y axes across the current logical view. */
void graph_draw_axes(void) {
  /* Ensure the projection/modelview matrices match our logical view. */
  graph_apply_view();
  glLineWidth(2.0f);
  glBegin(GL_LINES);
  glColor3f(1.0f, 1.0f, 1.0f);
  glVertex2f(gx_min, 0.0f);
  glVertex2f(gx_max, 0.0f);
  glVertex2f(0.0f, gy_min);
  glVertex2f(0.0f, gy_max);
  glEnd();
}

/** Draw a regular grid over the logical view.
 * @param x_step spacing between vertical grid lines (world units)
 * @param y_step spacing between horizontal grid lines (world units)
 */
void graph_draw_grid(float x_step, float y_step) {
  if (x_step <= 0.0f || y_step <= 0.0f)
    return;

  graph_apply_view();
  glLineWidth(1.0f);
  glColor3f(0.25f, 0.25f, 0.25f);
  glBegin(GL_LINES);

  float start_x = (int)floorf(gx_min / x_step) * x_step;
  for (float x = start_x; x <= gx_max; x += x_step) {
    glVertex2f(x, gy_min);
    glVertex2f(x, gy_max);
  }

  float start_y = (float)((int)floorf(gy_min / y_step)) * y_step;
  for (float y = start_y; y <= gy_max; y += y_step) {
    glVertex2f(gx_min, y);
    glVertex2f(gx_max, y);
  }

  glEnd();
}

/** Plot an array of points as a scatter plot.
 * @param pts array of points in world coordinates
 * @param n number of points
 * @param r,g,b color components (0..1)
 * @param size point size in pixels
 */
void graph_plot_points(const Point *pts, int n, float r, float g, float b,
                       float size) {
  if (!pts || n <= 0)
    return;

  graph_apply_view();
  glPointSize(size);
  glBegin(GL_POINTS);
  glColor3f(r, g, b);

  for (int i = 0; i < n; ++i)
    glVertex2f(pts[i].x, pts[i].y);
  glEnd();
}

/** Draw a connected polyline through the given points.
 * @param pts array of points in world coordinates
 * @param n number of points
 * @param r,g,b color components (0..1)
 * @param width visual line thickness in pixels
 */
void graph_plot_lines(const Point *pts, int n, float r, float g, float b,
                      float width) {
  if (!pts || n <= 0)
    return;

  graph_apply_view();
  glLineWidth(width);
  glColor3f(r, g, b);
  /* Begin a line strip: consecutive vertices are connected by segments. */
  glBegin(GL_LINE_STRIP);
  for (int i = 0; i < n; ++i)
    glVertex2f(pts[i].x, pts[i].y);
  glEnd();
}

/** Draw a bitmap text string at window coordinates (pixels).
 * Origin is bottom-left.
 * @param text null-terminated string
 * @param x window x in pixels (from left)
 * @param y window y in pixels (from bottom)
 */
void graph_draw_text(const char *text, int x, int y) {
  if (!text)
    return;

  glColor3f(1.0f, 1.0f, 1.0f);

  glWindowPos2i = (PFNGLWINDOWPOS2IPROC)glutGetProcAddress("glWindowPos2i");
  glWindowPos2i(x, y);

  for (const char *p = text; *p; ++p)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)*p);
}

/** Convenience: draw text anchored near the top-left of the window.
 * Adds a small padding from the edges.
 */
void graph_draw_text_top_left(const char *text) {
  if (!text)
    return;
  /* Padding from left and top in pixels */
  const int pad_x = 8;
  const int pad_y = 20; /* distance from top baseline */
  /* Window origin for our helper is bottom-left, so convert top-left y. */
  int win_y = g_win_h - pad_y;
  graph_draw_text(text, pad_x, win_y);
}


/*Draw numbers of graduation of axis*/
void graph_draw_numbers(float x_step, float y_step){
  graph_apply_view();

  float start_x = (int)floorf(gx_min / x_step) * x_step;
  for (float x = start_x; x <= gx_max; x += x_step) {
    if(x!=0){
      char nombre[32];
      snprintf(nombre, sizeof(nombre), "%g", x);
      if(x>0){
        int x_pixel, y_pixel;
        world_to_pixels(x-0.1f, -0.45f, &x_pixel, &y_pixel);
        graph_draw_text(nombre, x_pixel, y_pixel);
      }
      else{
        int x_pixel, y_pixel;
        world_to_pixels(x-0.38f, -0.45f, &x_pixel, &y_pixel);
        graph_draw_text(nombre, x_pixel, y_pixel);
      }
    }
  }

  float start_y = (float)((int)floorf(gy_min / y_step)) * y_step;
  for (float y = start_y; y <= gy_max; y += y_step) {
    if(y==0){
      char nombre[32];
      snprintf(nombre, sizeof(nombre), "%g", y);
      int x_pixel, y_pixel;
      world_to_pixels(-0.4f, -0.4f, &x_pixel, &y_pixel);
      graph_draw_text(nombre, x_pixel, y_pixel);
    }
    else {
      if(y>0){
        char nombre[32];
        snprintf(nombre, sizeof(nombre), "%g", y);
        int longeur = strlen(nombre);
        int x_pixel, y_pixel;
        world_to_pixels(-0.45f-0.1f*(longeur-1), y-0.1f, &x_pixel, &y_pixel);
        graph_draw_text(nombre, x_pixel, y_pixel);
      }
      else {
        char nombre[32];
        snprintf(nombre, sizeof(nombre), "%g", y);
        int longeur = strlen(nombre);
        int x_pixel, y_pixel;
        world_to_pixels(-0.45f-0.1f*longeur, y-0.1f, &x_pixel, &y_pixel);
        graph_draw_text(nombre, x_pixel, y_pixel);
      }
    }
  }
}




/*Draw small lines to graduate axis*/
void graph_draw_grid_min_lines(float x_step, float y_step) {
  if (x_step <= 0.0f || y_step <= 0.0f)
    return;

  graph_apply_view();
  glLineWidth(1.5f);
  glColor3f(1.0f, 1.0f, 1.0f);
  glBegin(GL_LINES);

  float start_x = (int)floorf(gx_min / x_step) * x_step;
  for (float x = start_x; x <= gx_max; x += x_step) {
    glVertex2f(x, -0.1f);
    glVertex2f(x, 0.1f);
  }

  float start_y = (float)((int)floorf(gy_min / y_step)) * y_step;
  for (float y = start_y; y <= gy_max; y += y_step) {
    glVertex2f(-0.1f, y);
    glVertex2f(0.1f, y);
  }

  glEnd();
}

/*Len charactere chain*/
int strlen(char s[]) {
    int i;
    i=0;
    while(s[i]!='\0')
        ++i;
    return i;
    }
