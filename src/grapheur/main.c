#include "./include/main.h"

static Point pts[NPOINTS];

/* Interaction state for pan/zoom */
static int g_dragging = 0;
static int g_last_x = 0, g_last_y = 0;

/** GLUT display callback: clear the buffer and render the scene. */
void display() {
  glClear(GL_COLOR_BUFFER_BIT);
  graph_draw_grid(.3f, .3f);
  graph_draw_axes();
  graph_plot_lines(pts, NPOINTS, 0.1f, 0.9f, 0.2f, 2.0f);

  /* Draw a small label in the top-left corner */
  graph_draw_text_top_left("sin");

  glFlush();
}

/** GLUT reshape callback: update cached window size and viewport. */
void graph_reshape(int w, int h) {
  if (w <= 0 || h <= 0)
    return;
  world_set_window_size(w, h);
  /* Update the GL viewport to match the new window size. */
  glViewport(0, 0, w, h);
}

/* Mouse button callback: handles wheel (button 3/4) and start/end drag */
/** Mouse button callback: handle wheel zoom and begin/end drag for panning.
 * Wheel events are reported as button 3 (up) / 4 (down) by GLUT.
 */
void mouse_button(int button, int state, int x, int y) {
  if (button == 3 || button == 4) {
    /* Wheel event: button 3 = up, 4 = down */
    float wx, wy;
    pixels_to_world(x, y, &wx, &wy);
    if (button == 3) {
      world_zoom_at(wx, wy, 1.2f); /* zoom in */
    } else {
      world_zoom_at(wx, wy, 1.0f / 1.2f); /* zoom out */
    }
    glutPostRedisplay();
  } else if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      g_dragging = 1;
      g_last_x = x;
      g_last_y = y;
    } else {
      g_dragging = 0;
    }
  }
}

/* Mouse motion callback while a button is pressed: perform pan */
/** Mouse motion callback while a button is pressed: perform pan.
 * Converts pixel deltas into world-space pan so the point under the cursor
 * remains fixed during dragging.
 */
void mouse_motion(int x, int y) {
  if (!g_dragging)
    return;
  /* Compute world coords for previous and current mouse positions */
  float wx_prev, wy_prev, wx_cur, wy_cur;
  pixels_to_world(g_last_x, g_last_y, &wx_prev, &wy_prev);
  pixels_to_world(x, y, &wx_cur, &wy_cur);
  /* Pan by the difference so the point under the cursor stays fixed */
  world_pan(wx_prev - wx_cur, wy_prev - wy_cur);
  g_last_x = x;
  g_last_y = y;
  glutPostRedisplay();
}

/** Program entry: initialize the window, set up data and register callbacks.
 * Enters the GLUT main loop and does not return under normal execution.
 */
int main(int argc, char **argv) {
  graph_init_window(&argc, argv, 1000, 700, "Calculatrice Graphique");
  graph_set_background(0.05f, 0.05f, 0.05f);
  world_set_view(-10.0f, 10.0f, -6.0f, 6.0f);

  for (int i = 0; i < NPOINTS; ++i) {
    float x = -100.0f + (200.0f * i) / (NPOINTS - 1);
    pts[i].x = x;
    pts[i].y = sinf(x);
  }
  /* Register callbacks for interaction */
  glutDisplayFunc(display);
  glutReshapeFunc(graph_reshape);
  glutMouseFunc(mouse_button);
  glutMotionFunc(mouse_motion);
  glutMainLoop();

  return 0;
}
