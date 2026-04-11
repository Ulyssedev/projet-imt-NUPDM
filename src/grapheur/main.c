#include "./include/main.h"
#include "../dialogueur/pipeline.h"
#include <GL/freeglut_std.h>
#include <stdio.h>
#include <stdlib.h>

static Point pts[NPOINTS];
static char g_expression[256] = "sin(x)";
static char g_status_message[PIPELINE_ERROR_MESSAGE_SIZE] = "";
static float g_xmin = -10.0f;
static float g_xmax = 10.0f;

/* Interaction state for pan/zoom */
static int g_dragging = 0;
static int g_last_x = 0, g_last_y = 0;
static int space_pressed = 0;

static void remplir_points_depuis_expression(void) {
  int i;
  int nb_erreurs = 0;

  g_status_message[0] = '\0';

  for (i = 0; i < NPOINTS; ++i) {
    float x = g_xmin + ((g_xmax - g_xmin) * i) / (NPOINTS - 1);
    float y = 0.0f;
    char erreur[PIPELINE_ERROR_MESSAGE_SIZE] = "";
    int status = calculer_fx(g_expression, x, &y, erreur, sizeof(erreur));

    pts[i].x = x;
    if (status == PIPELINE_OK) {
      pts[i].y = y;
    } else {
      pts[i].y = 0.0f;
      nb_erreurs++;
      if (g_status_message[0] == '\0') {
        snprintf(g_status_message, sizeof(g_status_message),
                 "Erreur calcul: %.480s", erreur);
      }
    }
  }

  if (nb_erreurs > 0 && g_status_message[0] == '\0') {
    snprintf(g_status_message, sizeof(g_status_message),
             "Erreur calcul sur %d points", nb_erreurs);
  }
}

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

  if (g_status_message[0] != '\0') {
    graph_draw_text_top_left(g_status_message);
  } else {
    graph_draw_text_top_left(g_expression);
  }

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
  if (argc >= 2) {
    snprintf(g_expression, sizeof(g_expression), "%s", argv[1]);
  }
  if (argc >= 4) {
    g_xmin = strtof(argv[2], NULL);
    g_xmax = strtof(argv[3], NULL);
    if (g_xmin >= g_xmax) {
      g_xmin = -10.0f;
      g_xmax = 10.0f;
    }
  }

  remplir_points_depuis_expression();

  graph_init_window(&argc, argv, 1000, 700,
                    "Calculatrice Graphique : GraphitXcalc");
  graph_set_background(0.05f, 0.05f, 0.05f);
  world_set_view(g_xmin, g_xmax, -6.0f, 6.0f);
  /* Register callbacks for interaction */
  glutDisplayFunc(display);
  glutReshapeFunc(graph_reshape);
  glutMouseFunc(mouse_button);
  glutMotionFunc(mouse_motion);
  glutKeyboardFunc(keyboard_button);
  glutMainLoop();

  return 0;
}
