#include "main.h"

static Point pts[NPOINTS];

static void fill_default_sine(void) {
  float x0 = trace_x_locked ? trace_gx_min : gx_min;
  float x1 = trace_x_locked ? trace_gx_max : gx_max;
  float span = x1 - x0;

  if (NPOINTS <= 1) {
    pts[0].x = x0;
    pts[0].y = sinf(x0);
    return;
  }

  for (int i = 0; i < NPOINTS; ++i) {
    float x = x0 + (span * i) / (NPOINTS - 1);
    pts[i].x = x;
    pts[i].y = sinf(x);
  }
}

static void fill_function_points(Arbre root) {
  float x0 = trace_x_locked ? trace_gx_min : gx_min;
  float x1 = trace_x_locked ? trace_gx_max : gx_max;
  float span = x1 - x0;

  if (NPOINTS <= 1) {
    pts[0].x = x0;
    pts[0].y = Eval(root, x0);
    return;
  }

  for (int i = 0; i < NPOINTS; ++i) {
    float x = x0 + (span * i) / (NPOINTS - 1);
    pts[i].x = x;
    pts[i].y = Eval(root, x);
  }
}

/** Callback d'affichage GLUT : efface le buffer et dessine la scène. */
void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  graph_draw_grid(1.0f, 1.0f);
  graph_draw_grid_min_lines(1.0f, 1.0f);
  graph_draw_numbers(1.0f, 1.0f);
  graph_draw_axes();

  int count = menu_get_count();
  if (count == 0) {
    fill_default_sine();
    graph_plot_lines(pts, NPOINTS, 0.1f, 0.9f, 0.2f, 2.0f);
  } else {
    for (int fi = 0; fi < count; ++fi) {
      Arbre root = menu_get_cached_arbre(fi);
      if (!root)
        continue;

      fill_function_points(root);
      const float *col = palette[fi % PALETTE_COUNT];
      graph_plot_lines(pts, NPOINTS, col[0], col[1], col[2], 2.0f);
    }
  }

  if (show_coords == 1) {
    graph_draw_coords_top_right(saved_world_x, saved_world_y);
  }
  if (draw_coords == 1) {
    graph_draw_coords_red_lines(saved_world_x, saved_world_y);
  }

  menu_draw_button();
  menu_draw_overlay();

  glFlush();
}

/** Callback de redimensionnement GLUT : met à jour la taille en pixels mise
 * en cache et le viewport OpenGL.
 */
void graph_reshape(int w, int h) {
  if (w <= 0 || h <= 0)
    return;

  world_set_window_size(w, h);
  glViewport(0, 0, w, h);
}

/* Callback d'idle : redessine quand le menu change. */
static void idle_cb(void) {
  if (menu_is_updated()) {
    menu_clear_updated_flag();
    glutPostRedisplay();
  }
}

/** Entrée du programme : initialise la fenêtre, configure les données et
 * enregistre les callbacks. Entre ensuite dans la boucle principale GLUT.
 */
int main(int argc, char **argv) {
  (void)argv;

  graph_init_window(&argc, argv, 1000, 700,
                    "Calculatrice Graphique : GraphitXcalc");
  graph_set_background(0.05f, 0.05f, 0.05f);
  world_set_view(-10.0f, 10.0f, -6.0f,
                 6.0f); // 6 environ égal à 9/16*10 pour un rendu plus naturel
                        // sur des écrans 16:9

  menu_init();
  entry_init();

  fill_default_sine();

  glutDisplayFunc(display);
  glutReshapeFunc(graph_reshape);
  glutIdleFunc(idle_cb);
  glutMainLoop();

  return 0;
}
