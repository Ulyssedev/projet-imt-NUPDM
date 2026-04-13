#include "entry.h"

/* État d'interaction pour le pan/zoom */
static int g_dragging = 0;
static int g_last_x = 0, g_last_y = 0;

int menu_handle_key(unsigned char key, int x, int y) {
  (void)x;
  (void)y;
  /* bascule globale */
  if (key == 'm' || key == 'M') {
    menu_toggle_visible();
    return 1;
  }
  if (!menu_visible)
    return 0;

  /* Edition des bornes (mode dédié: 1=X, 2=Y) */
  if (menu_edit_bounds_mode != 0) {
    if (key == 13 || key == '\n' || key == '\r') {
      /* valider et parser deux floats min/max */
      float new_min1 = 0.0f, new_max1 = 0.0f;
      int n = sscanf(menu_bounds_input, "%f %f", &new_min1, &new_max1);
      if (n == 2) {
        if (menu_edit_bounds_mode == 1) {
          gx_min = new_min1;
          gx_max = new_max1;
        } else {
          gy_min = new_min1;
          gy_max = new_max1;
        }
      }
      menu_edit_bounds_mode = 0;
      menu_bounds_input_pos = 0;
      menu_bounds_input[0] = '\0';
      return 1;
    }
    if (key == 27) { /* échap */
      menu_edit_bounds_mode = 0;
      menu_bounds_input_pos = 0;
      menu_bounds_input[0] = '\0';
      return 1;
    }
    if (key == 8 || key == 127) { /* retour arrière */
      if (menu_bounds_input_pos > 0) {
        menu_bounds_input[--menu_bounds_input_pos] = '\0';
      }
      return 1;
    }
    if (isprint((unsigned char)key) && menu_bounds_input_pos + 1 < MENU_BOUNDS_INPUT_LEN) {
      menu_bounds_input[menu_bounds_input_pos++] = key;
      menu_bounds_input[menu_bounds_input_pos] = '\0';
    }
    return 1;
  }

  if (menu_editing) {
    if (key == 13 || key == '\n' || key == '\r') {
      /* valider */
      if (menu_adding) {
        if (menu_input_pos > 0)
          menu_add_function(menu_input);
      } else {
        menu_edit_function(menu_selected, menu_input);
      }
      menu_editing = 0;
      menu_adding = 0;
      menu_input_pos = 0;
      menu_input[0] = '\0';
      return 1;
    }
    if (key == 27) { /* échap */
      menu_editing = 0;
      menu_adding = 0;
      menu_input_pos = 0;
      menu_input[0] = '\0';
      return 1;
    }
    if (key == 8 || key == 127) { /* retour arrière */
      if (menu_input_pos > 0) {
        menu_input[--menu_input_pos] = '\0';
      }
      return 1;
    }
    if (isprint((unsigned char)key) && menu_input_pos + 1 < MENU_MAX_FUNC_LEN) {
      menu_input[menu_input_pos++] = key;
      menu_input[menu_input_pos] = '\0';
    }
    return 1;
  }

  /*
   * Clavier (mode édition) :
   * - Entrée : détection via 13 / '\n' / '\r' (divers environnements peuvent
   * générer l'un ou l'autre) ; on valide la saisie.
   * - Échap (27) : annule l'édition.
   * - Retour arrière : certains systèmes renvoient 8, d'autres 127 ; on gère
   * les deux valeurs.
   * - isprint : n'ajoute que les caractères imprimables dans le buffer.
   *
   * Remarque : le callback clavier de GLUT fournit un `unsigned char` pour les
   * touches normales, ce qui correspond bien à ce test ASCII.
   */

  /* hors édition : navigation et commandes */
  if (key == 'a' || key == 'A') {
    menu_adding = 1;
    menu_editing = 1;
    menu_input_pos = 0;
    menu_input[0] = '\0';
    return 1;
  }
  if (key == 'e' || key == 'E') {
    if (menu_selected >= 0 && menu_selected < func_count) {
      strncpy(menu_input, funcs[menu_selected], MENU_MAX_FUNC_LEN - 1);
      menu_input[MENU_MAX_FUNC_LEN - 1] = '\0';
      menu_input_pos = strlen(menu_input);
      menu_editing = 1;
      menu_adding = 0;
    }
    return 1;
  }
  if (key == 'd' || key == 'D') {
    if (menu_selected >= 0 && menu_selected < func_count) {
      menu_remove_function(menu_selected);
      if (menu_selected >= func_count)
        menu_selected = func_count > 0 ? func_count - 1 : 0;
    }
    return 1;
  }
  if (key == 'q' || key == 'Q') {
    menu_set_visible(0);
    return 1;
  }

  /* Démarrer l'édition des bornes X via 'r' ou Y via 'y' */
  if (key == 'r' || key == 'R' || key == 'y' || key == 'Y') {
    if (key == 'r' || key == 'R')
      menu_edit_bounds_mode = 1;
    else
      menu_edit_bounds_mode = 2;
    if (menu_edit_bounds_mode == 1)
      snprintf(menu_bounds_input, MENU_BOUNDS_INPUT_LEN, "%g %g", gx_min, gx_max);
    else
      snprintf(menu_bounds_input, MENU_BOUNDS_INPUT_LEN, "%g %g", gy_min, gy_max);
    int n = (int)strlen(menu_bounds_input);
    if (n < 0)
      menu_bounds_input_pos = 0;
    else if (n >= MENU_BOUNDS_INPUT_LEN)
      menu_bounds_input_pos = MENU_BOUNDS_INPUT_LEN - 1;
    else
      menu_bounds_input_pos = n;
    menu_bounds_input[menu_bounds_input_pos] = '\0';
    return 1;
  }

  return 0;
}

/** Callback de mouvement souris pendant un drag : réalise le panoramique.
 * On convertit les deltas en pixels en offsets en coordonnées world afin que
 * le point sous le curseur reste fixe pendant le déplacement.
 */
void mouse_motion(int x, int y) {
  if (!g_dragging)
    return;
  /* coordonnées world des positions précédente et courante du curseur */
  float wx_prev, wy_prev, wx_cur, wy_cur;
  pixels_to_world(g_last_x, g_last_y, &wx_prev, &wy_prev);
  pixels_to_world(x, y, &wx_cur, &wy_cur);
  /* pan par la différence pour conserver le point sous le curseur */
  world_pan(wx_prev - wx_cur, wy_prev - wy_cur);
  g_last_x = x;
  g_last_y = y;
  glutPostRedisplay();
}

/** Callback sur les boutons de souris : gère la molette (zoom) et le début/fin
 * d'un drag pour le panoramique.
 * Note : GLUT rapporte la molette comme button 3 (up) / 4 (down).
 */
void mouse_button(int button, int state, int x, int y) {
  /* laisser le menu gérer les clics dans son aire (coin sup-gauche) */
  if (menu_handle_mouse(button, state, x, y)) {
    glutPostRedisplay();
    return;
  }
  if (button == 3 || button == 4) {
    /* événement molette */
    float wx, wy;
    pixels_to_world(x, y, &wx, &wy);
    if (button == 3) {
      world_zoom_at(wx, wy, 1.2f); /* zoom avant */
    } else {
      world_zoom_at(wx, wy, 1.0f / 1.2f); /* zoom arrière */
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

/** Gestionnaire de touches clavier (ASCII) : délègue au menu ou gère des
 * raccourcis locaux (ex : espace pour sauvegarder coordonnées, 't' pour
 * basculer l'affichage des lignes rouges).
 */
void keyboard_button(unsigned char key, int x, int y) {
  if (menu_handle_key(key, x, y)) {
    glutPostRedisplay();
    return;
  }

  if (key == ' ') {
    pixels_to_world(x, y, &saved_world_x, &saved_world_y);
    show_coords = 1;
  }
  if (key == 't') {
    if (draw_coords == 0) {
      draw_coords = 1;
    } else {
      draw_coords = 0;
    }
  }
  /* Zoom avant/arrière via clavier: '=' or '+' => zoom in, '-' or ')' => zoom out */
  if (key == '=' || key == '+') {
    world_zoom_at((gx_min + gx_max)/2, (gy_min + gy_max)/2, 1.2f);
  }
  if (key == '-' || key == ')') {
    world_zoom_at((gx_min + gx_max)/2, (gy_min + gy_max)/2, 1.0f / 1.2f);
  }
  glutPostRedisplay();
}

void special_button(int key, int x, int y) {
  if (menu_handle_special(key, x, y)) {
    glutPostRedisplay();
  }
}

int menu_handle_mouse(int button, int state, int x, int y) {
  /* x,y sont avec origine en haut à gauche ; conversion en bas à gauche pour
   * l'overlay */
  if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
    return 0;
  const int pad = 8;
  const int width = 72;
  const int height = 28;
  int left = pad;
  int top = g_win_h - pad;
  int right = left + width;
  int bottom = top - height;
  int by = g_win_h - y; /* conversion en origine bas-gauche */
  if (x >= left && x <= right && by >= bottom && by <= top) {
    menu_toggle_visible();
    return 1;
  }
  return 0;
}

/*
 * Note sur `menu_handle_mouse` :
 * - GLUT fournit généralement les coordonnées de la souris avec l'origine en
 *   haut à gauche de la fenêtre; l'overlay utilise une origine bas-gauche
 *   (pour correspondre à la projection orthographique ci-dessus), d'où la
 *   conversion `by = g_win_h - y`.
 * - Le test ci-dessus vérifie si le clic se trouve dans le petit bouton
 *   "Menu" en haut-gauche et bascule la visibilité si oui.
 */

int menu_handle_special(int key, int x, int y) {
  (void)x;
  (void)y;
  int dx = 0, dy = 0;

  if (!menu_visible) {
    if (key == GLUT_KEY_UP)
      dy = 10;
    if (key == GLUT_KEY_DOWN)
      dy = -10;
    if (key == GLUT_KEY_LEFT)
      dx = 10;
    if (key == GLUT_KEY_RIGHT)
      dx = -10;

    if (dx != 0 || dy != 0) {
      g_dragging = 1;
      mouse_motion(g_last_x + dx, g_last_y + dy);
    }

    g_dragging = 0;

    return 0;
  }

  if (menu_editing)
    return 0; /* on laisse les touches spéciales au contexte d'édition si besoin
               */
  int count = func_count;
  if (key == GLUT_KEY_UP) {
    if (menu_selected > 0)
      menu_selected--;
    return 1;
  } else if (key == GLUT_KEY_DOWN) {
    if (menu_selected + 1 < count)
      menu_selected++;
    return 1;
  }
  return 0;
}

void entry_init(void) {
  glutKeyboardFunc(keyboard_button);
  glutSpecialFunc(special_button);
}
