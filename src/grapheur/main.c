#include "./include/main.h"
#include <GL/freeglut_std.h>

static Point pts[NPOINTS];

/* Interaction state for pan/zoom */
static int g_dragging = 0;
static int g_last_x = 0, g_last_y = 0;
static int space_pressed = 0;

/** GLUT display callback: clear the buffer and render the scene. */
void display() {
  glClear(GL_COLOR_BUFFER_BIT);
  graph_draw_grid(1.0f, 1.0f);
  graph_draw_grid_min_lines(1.0f, 1.0f);
  graph_draw_numbers(1.0f, 1.0f);
  graph_draw_axes();
  graph_plot_lines(pts, NPOINTS, 0.1f, 0.9f, 0.2f, 2.0f);

  if (show_coords == 1) {
    graph_draw_coords_top_right(saved_world_x, saved_world_y);
  }
  if (draw_coords == 1) {
    graph_draw_coords_red_lines(saved_world_x, saved_world_y);
  }
  
  /* Draw a small label in the top-left corner */
  graph_draw_text_top_left("sin");

  glFlush();
}

/** GLUT reshabpe callack: update cached window size and viewport. */
void graph_reshape(int w, int h) {
  if (w <= 0 || h <= 0)
    return;
  world_set_window_size(w, h);
  /* Update the GL viewport to match the new window size. */
  glViewport(0, 0, w, h);
}

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

/** Mouse motion callback while a button is pressed: perform pan.
 * Converts pixel deltas into world-space pan so the point under the cursor
 * remains fixed during dragging.
 */
void keyboard_button(unsigned char key, int x, int y) {
  if (key == ' ') {
    pixels_to_world(x, y, &saved_world_x, &saved_world_y);
    show_coords = 1;
  }
  if (key == 't') {
    if (draw_coords == 0){draw_coords=1;}
    else {draw_coords=0;}
  }
  glutPostRedisplay();
}


/** Program entry: initialize the window, set up data and register callbacks.
 * Enters the GLUT main loop and does not return under normal execution.
 */
int main(int argc, char **argv) {
  graph_init_window(&argc, argv, 1000, 700,
                    "Calculatrice Graphique : GraphitXcalc");
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
  glutKeyboardFunc(keyboard_button);
  glutMainLoop();

  return 0;
}
