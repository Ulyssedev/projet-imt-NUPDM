#include "menu.h"

/*
 * menu.c - Menu visuel pour GraphitX (GLUT)
 *
 * Ce module gère un petit menu textuel superposé à l'affichage OpenGL :
 *  - stockage des expressions utilisateur dans `funcs`
 *  - cache optionnel d'arbres syntaxiques (`func_trees`) construits via
 *    l'API `pipeline_*` pour un rendu immédiat
 *  - fonctions d'ajout/édition/suppression et gestion des saisies
 * clavier/souris
 *  - dessin d'une surcouche semi-transparente et d'un petit bouton pour
 *    basculer l'affichage du menu
 *
 * Le module est conçu pour être utilisé dans la boucle principale GLUT
 * (thread unique). Les arbres stockés dans `func_trees` sont détenus par
 * ce module et libérés dans `menu_shutdown` ou lors d'une modification.
 */

char funcs[MENU_MAX_FUNCS][MENU_MAX_FUNC_LEN];
/* Cached parsed ASTs (Arbre) for each function string. May be NULL. */
static Arbre func_trees[MENU_MAX_FUNCS];
/* Number of functions currently stored in `funcs`/`func_trees`. */
int func_count = 0;
/* Flag set when menu content/state changed; consumer should call
 * `menu_clear_updated_flag` after reacting. */
static int updated = 0;

/*
 * `updated` : indicateur simple pour informer le reste de l'application
 * qu'un changement interne est survenu (ajout/édition/suppression/visibilité)
 * Le consommateur (ex: boucle de rendu) appelle `menu_is_updated()` et
 * `menu_clear_updated_flag()` pour synchroniser l'affichage.
 */

/* ---------------- Visual menu (GLUT) state ---------------- */
/* Visuel/menu état de l'UI utilisé dans les callbacks de GLUT. */
int menu_visible = 0;
int menu_selected = 0;
/* `true` lorsque l'utilisateur modifie ou ajoute une fonction */
int menu_editing = 0;
/* `true` lorsque l'utilisateur est entrain d'ajouter une fonction (false =
 * edit) */
int menu_adding = 0;
/* entrée buffer pour modifier/ajouter la chaîne de caractère d'une fonction */
char menu_input[MENU_MAX_FUNC_LEN];
int menu_input_pos = 0;
/* Mode d'édition des bornes de la vue (0=none, 1=edit X, 2=edit Y) */
int menu_edit_bounds_mode = 0;
char menu_bounds_input[MENU_BOUNDS_INPUT_LEN];
int menu_bounds_input_pos = 0;
static char menu_error_messages[MENU_MAX_FUNCS][256];
static unsigned char menu_error_present[MENU_MAX_FUNCS];
static const int menu_line_h = 20;

static int menu_text_width(const char *text) {
  int width = 0;

  if (text == NULL)
    return 0;

  for (const char *p = text; *p; ++p) {
    width += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, (int)*p);
  }

  return width;
}

/* Calcule le nombre de lignes nécessaires pour un texte découpé selon une
 * largeur maximale en pixels. */
static int menu_wrapped_line_count(const char *text, int max_width) {
  const char *cursor;
  int lines = 0;

  if (text == NULL || max_width <= 0)
    return 1;

  cursor = text;
  while (*cursor != '\0') {
    char word[256];
    int line_width = 0;
    int has_content = 0;

    while (*cursor == ' ')
      cursor++;
    if (*cursor == '\0')
      break;

    while (*cursor != '\0' && *cursor != '\n') {
      const char *word_start = cursor;
      size_t word_len;
      int word_width;
      int space_width;

      while (*cursor != '\0' && *cursor != ' ' && *cursor != '\n')
        cursor++;

      word_len = (size_t)(cursor - word_start);
      if (word_len == 0)
        break;
      if (word_len >= sizeof(word))
        word_len = sizeof(word) - 1;

      memcpy(word, word_start, word_len);
      word[word_len] = '\0';
      word_width = menu_text_width(word);
      space_width = has_content ? menu_text_width(" ") : 0;

      if (has_content && line_width + space_width + word_width > max_width) {
        lines++;
        line_width = 0;
        has_content = 0;
        space_width = 0;
      }

      if (has_content)
        line_width += space_width;
      line_width += word_width;
      has_content = 1;

      while (*cursor == ' ')
        cursor++;
      if (*cursor == '\n')
        break;
    }

    if (has_content)
      lines++;

    while (*cursor == ' ' || *cursor == '\n')
      cursor++;
  }

  return lines > 0 ? lines : 1;
}

/* Dessine un texte multi-lignes, en conservant la couleur OpenGL courante. */
static int menu_draw_wrapped_text(const char *text, int x, int y,
                                  int max_width) {
  const char *cursor;
  int lines_drawn = 0;

  if (text == NULL || max_width <= 0) {
    graph_draw_text_current_color("", x, y);
    return 1;
  }

  cursor = text;
  while (*cursor != '\0') {
    char line[512];
    int line_width = 0;
    int has_content = 0;

    while (*cursor == ' ')
      cursor++;
    if (*cursor == '\0')
      break;

    line[0] = '\0';
    while (*cursor != '\0' && *cursor != '\n') {
      const char *word_start = cursor;
      size_t word_len;
      char word[256];
      int word_width;
      int space_width;

      while (*cursor != '\0' && *cursor != ' ' && *cursor != '\n')
        cursor++;

      word_len = (size_t)(cursor - word_start);
      if (word_len == 0)
        break;
      if (word_len >= sizeof(word))
        word_len = sizeof(word) - 1;

      memcpy(word, word_start, word_len);
      word[word_len] = '\0';
      word_width = menu_text_width(word);
      space_width = has_content ? menu_text_width(" ") : 0;

      if (has_content && line_width + space_width + word_width > max_width) {
        graph_draw_text_current_color(line, x, y - lines_drawn * menu_line_h);
        lines_drawn++;
        line[0] = '\0';
        line_width = 0;
        has_content = 0;
        space_width = 0;
      }

      if (has_content) {
        strncat(line, " ", sizeof(line) - strlen(line) - 1);
        line_width += space_width;
      }
      strncat(line, word, sizeof(line) - strlen(line) - 1);
      line_width += word_width;
      has_content = 1;

      while (*cursor == ' ')
        cursor++;
      if (*cursor == '\n')
        break;
    }

    if (has_content) {
      graph_draw_text_current_color(line, x, y - lines_drawn * menu_line_h);
      lines_drawn++;
    }

    while (*cursor == ' ' || *cursor == '\n')
      cursor++;
  }

  return lines_drawn > 0 ? lines_drawn : 1;
}

static void menu_clear_error_slot(int idx) {
  if (idx < 0 || idx >= MENU_MAX_FUNCS)
    return;
  menu_error_present[idx] = 0;
  menu_error_messages[idx][0] = '\0';
}

static void menu_clear_all_errors(void) {
  for (int i = 0; i < MENU_MAX_FUNCS; ++i)
    menu_clear_error_slot(i);
}

static void menu_set_pipeline_error_message(int idx, int pcode,
                                            const lexical_error_t *lerr,
                                            int syntax_err) {
  const char *message = NULL;

  if (idx < 0 || idx >= MENU_MAX_FUNCS)
    return;

  switch (pcode) {
  case PIPELINE_ERREUR_LEXICALE:
    if (lerr != NULL && lerr->message[0] != '\0') {
      message = lerr->message;
    } else {
      message = "erreur lexicale";
    }
    break;
  case PIPELINE_ERREUR_SYNTAXIQUE:
    switch (syntax_err) {
    case SYNTAXE_ERREUR_GRAMMAIRE:
      message = "erreur syntaxique : grammaire invalide";
      break;
    case SYNTAXE_ERREUR_FIN_MANQUANTE:
      message = "erreur syntaxique : fin manquante";
      break;
    default:
      message = "erreur syntaxique";
      break;
    }
    break;
  case PIPELINE_ERREUR_MEMOIRE:
    message = "allocation memoire impossible";
    break;
  case PIPELINE_ERREUR_ARGUMENT:
    message = "expression invalide";
    break;
  default:
    message = "erreur inconnue";
    break;
  }

  menu_error_present[idx] = 1;
  snprintf(menu_error_messages[idx], sizeof(menu_error_messages[idx]), "%s",
           message);
}

void menu_init(void) {
  /* Reset tous les états. A appeler une seule fois dans le programme. */
  func_count = 0;
  for (int i = 0; i < MENU_MAX_FUNCS; ++i)
    func_trees[i] = NULL;
  updated = 1; /* forcer le premier traçage d'une fonction, ici sinus */
  menu_visible = 0;
  menu_selected = 0;
  menu_editing = 0;
  menu_adding = 0;
  menu_input_pos = 0;
  menu_input[0] = '\0';
  menu_bounds_input_pos = 0;
  menu_edit_bounds_mode = 0;
  menu_bounds_input[0] = '\0';
  menu_clear_all_errors();
  glutMouseFunc(mouse_button);
  glutMotionFunc(mouse_motion);
  glutPassiveMotionFunc(mouse_motion);
}

void menu_shutdown(void) {
  /* Libère les arbres stockés dans le cache de ce module.
   * Note: `liberer_arbre_pipeline` doit être appelé pour libérer la mémoire
   * alloué à `pipeline_build_arbre`. Après libération, on change le slot
   * en NULL pour éviter les pointeurs pointent dans des zones non
   * demandés/accessibles. */
  for (int i = 0; i < func_count; ++i) {
    if (func_trees[i])
      liberer_arbre_pipeline(func_trees[i]);
    func_trees[i] = NULL;
  }
}

int menu_get_count(void) { return func_count; }

void menu_get_function(int idx, char *out, int out_size) {
  if (idx < 0 || idx >= func_count) {
    if (out_size > 0)
      out[0] = '\0';
    return;
  }
  /* Safe copie de la chaîne de caractère de la fonction stocké dans le buffer.
   * Nous terminons toujours par un caractère nul et évitons de dépasser la
   * valeur de `out`. L'appelant donne un buffer de taille appropriée. */
  strncpy(out, funcs[idx], out_size - 1);
  out[out_size - 1] = '\0';
}

int menu_add_function(const char *func_str) {
  if (func_count >= MENU_MAX_FUNCS)
    return -1;
  /* Stocke la chaîne et tente de construire son AST immédiatement pour que le
   * grapheur puisse l'afficher sans délai. Étapes : 1) copie l'expression
   * textuelle dans notre tableau `funcs` (copie bornée) 2) tente de parser et
   * construire un AST via `pipeline_build_arbre`
   *     - `root` reçoit l'arbre si tout va bien
   *     - on stocke l'arbre dans `func_trees` uniquement si `PIPELINE_OK`
   *  3) incrémente `func_count` et signale la mise à jour
   *
   * Important : si la construction de l'arbre échoue, on conserve quand même la
   * chaîne afin que l'utilisateur puisse l'éditer ou tenter à nouveau.
   */
  strncpy(funcs[func_count], func_str, MENU_MAX_FUNC_LEN - 1);
  funcs[func_count][MENU_MAX_FUNC_LEN - 1] = '\0';
  int rc = func_count;
  func_trees[rc] = NULL;

  /* tente de construire l'AST maintenant pour qu'il soit disponible pour un
   * affichage immédiat */
  Arbre root = NULL;
  lexical_error_t lerr = {0};
  int syntax_err = 0;
  int pcode = pipeline_build_arbre(funcs[rc], &root, &lerr, &syntax_err);
  if (pcode == PIPELINE_OK) {
    func_trees[rc] = root;
    menu_clear_error_slot(rc);
  } else {
    menu_set_pipeline_error_message(rc, pcode, &lerr, syntax_err);
  }
  func_count++;
  updated = 1;
  return rc;
}

int menu_edit_function(int idx, const char *func_str) {
  if (idx < 0 || idx >= func_count)
    return -1;
  /* Remplace la chaîne stockée et reconstruit l'AST en cache. Tout AST
   * précédent est libéré. Note sur la propriété : `func_trees[idx]` (si non
   * NULL) est libéré ici car le nouvel arbre remplace l'ancien. En cas d'échec
   * du parsing, la case restera NULL et la chaîne modifiée sera conservée. */
  strncpy(funcs[idx], func_str, MENU_MAX_FUNC_LEN - 1);
  funcs[idx][MENU_MAX_FUNC_LEN - 1] = '\0';
  /* libère l'ancien AST s'il existe */
  if (func_trees[idx]) {
    liberer_arbre_pipeline(func_trees[idx]);
    func_trees[idx] = NULL;
  }
  /* reconstruit l'AST pour l'expression éditée */
  Arbre root = NULL;
  lexical_error_t lerr = {0};
  int syntax_err = 0;
  int pcode = pipeline_build_arbre(funcs[idx], &root, &lerr, &syntax_err);
  if (pcode == PIPELINE_OK) {
    func_trees[idx] = root;
    menu_clear_error_slot(idx);
  } else {
    menu_set_pipeline_error_message(idx, pcode, &lerr, syntax_err);
  }
  updated = 1;
  return idx;
}

int menu_remove_function(int idx) {
  if (idx < 0 || idx >= func_count)
    return -1;
  /* Supprime l'entrée : libère l'AST, décale les tableaux et met à jour la
   * sélection. On décale les entrées suivantes vers la gauche pour garder les
   * tableaux compacts. La dernière case est mise à NULL pour éviter de pointer
   * vers de la mémoire libérée. L'index sélectionné est ajusté pour rester dans
   * la plage après suppression. */
  if (func_trees[idx])
    liberer_arbre_pipeline(func_trees[idx]);
  for (int i = idx; i + 1 < func_count; ++i) {
    strncpy(funcs[i], funcs[i + 1], MENU_MAX_FUNC_LEN);
    func_trees[i] = func_trees[i + 1];
    menu_error_present[i] = menu_error_present[i + 1];
    strncpy(menu_error_messages[i], menu_error_messages[i + 1],
            sizeof(menu_error_messages[i]) - 1);
    menu_error_messages[i][sizeof(menu_error_messages[i]) - 1] = '\0';
  }
  func_trees[func_count - 1] = NULL;
  menu_clear_error_slot(func_count - 1);
  func_count--;
  if (menu_selected >= func_count)
    menu_selected = func_count > 0 ? func_count - 1 : 0;
  updated = 1;
  return 0;
}

int menu_is_updated(void) { return updated; }

void menu_clear_updated_flag(void) { updated = 0; }

void menu_toggle_visible(void) { menu_set_visible(!menu_visible); }

void menu_set_visible(int visible) {
  menu_visible = visible ? 1 : 0;
  if (menu_selected >= func_count)
    menu_selected = func_count > 0 ? func_count - 1 : 0;
  updated = 1;
}

void menu_draw_overlay(void) {
  /* Dessine la surcouche de menu translucide si visible. Les coordonnées
   * utilisées ici sont en pixels de fenêtre (origine en bas à gauche), donc on
   * définit une projection orthographique pour dessiner les éléments d'UI
   * directement en pixels. */
  if (!menu_visible)
    return;

  const int pad = 10;
  const int width = 750;
  const int text_width = width - 48;
  int error_lines = 0;
  int footer_lines = (menu_editing || menu_edit_bounds_mode != 0) ? 1 : 0;
  for (int i = 0; i < func_count; ++i) {
    if (menu_error_present[i]) {
      char error_line[320];
      snprintf(error_line, sizeof(error_line), "Erreur [%d] : %s", i,
               menu_error_messages[i]);
      error_lines += menu_wrapped_line_count(error_line, text_width);
    }
  }
  int lines =
      8 + footer_lines + (func_count > 0 ? func_count : 0) + error_lines;
  int height = 40 + lines * menu_line_h;
  if (height > g_win_h - 2 * pad)
    height = g_win_h - 2 * pad;

  int left = pad;
  int top = g_win_h - pad;
  int right = left + width;
  int bottom = top - height;

  /* Prépare la projection orthographique en pixels pour le dessin de l'UI :
   * - glMatrixMode(GL_PROJECTION) : sélectionne la matrice de projection pour
   * que les appels suivants la modifient
   * - glPushMatrix() : sauvegarde la projection courante pour la restaurer
   * ensuite (important pour ne pas perturber la caméra 2D/3D de l'application)
   * - glLoadIdentity() : réinitialise la projection avant d'appliquer glOrtho
   * - glOrtho(0, g_win_w, 0, g_win_h, -1, 1) : crée une projection
   * orthographique où les coordonnées correspondent aux pixels de la fenêtre,
   * origine en bas à gauche
   * - On passe ensuite en GL_MODELVIEW et on push/load identity pour isoler les
   * transformations UI du reste de la scène
   *
   * Note : on push les deux matrices (PROJECTION et MODELVIEW) et on doit les
   * pop dans l'ordre inverse pour restaurer complètement l'état OpenGL (voir la
   * fin de la fonction)
   */
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, g_win_w, 0, g_win_h, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  /*
   * Important : on push les deux matrices (PROJECTION et MODELVIEW) et on les
   * restaure à la fin de la fonction. Cela garantit que le dessin de l'UI ne
   * pollue pas les transformations 2D/3D du reste de l'application.
   */

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /* Fond */
  /* Fond : dessine un quad translucide qui remplit la surcouche. Les sommets
   * sont spécifiés en pixels grâce à la projection orthographique. Le canal
   * alpha de glColor4f contrôle la transparence du fond. */
  glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
  glBegin(GL_QUADS);
  glVertex2f(left, bottom);
  glVertex2f(left, top);
  glVertex2f(right, top);
  glVertex2f(right, bottom);
  glEnd();

  /* Bordure */
  /* Bordure : dessine un contour blanc avec GL_LINE_LOOP. Les primitives de
   * ligne utilisent aussi le repère en pixels grâce à l'ortho. glLineWidth
   * affecte l'épaisseur mais dépend de la plateforme. */
  glColor3f(1.0f, 1.0f, 1.0f);
  glLineWidth(1.0f);
  glBegin(GL_LINE_LOOP);
  glVertex2f(left, bottom);
  glVertex2f(left, top);
  glVertex2f(right, top);
  glVertex2f(right, bottom);
  glEnd();

  /* Titre */
  graph_draw_text(
      "GraphitXCalc - Menu visuel (a:ajouter e:editer d:suppr m:masquer)",
      left + 8, top - 22);
  graph_draw_text(
      "(clic-droit:suivre espace:coos haut droite t:ligne rouge sur coos)",
      left + 8, top - 22 - menu_line_h);
  graph_draw_text("(haut/bas:selection t:ligne sur coos)", left + 8,
                  top - 22 - 2 * menu_line_h);
  graph_draw_text(
      "Hors menu : haut/bas/gauche/droite:deplacement scroll/)/=:zoom",
      left + 8, top - 22 - 3 * menu_line_h);

  /* Bornes de la vue (affichage + hint d'édition) */

  char bounds_line[128];
  snprintf(bounds_line, sizeof(bounds_line),
           "Vue X: [%g, %g]  Y: [%g, %g]  (r:editer X, y:editer Y)", gx_min,
           gx_max, gy_min, gy_max);
  glColor3f(0.9f, 0.9f, 0.9f);
  graph_draw_text(bounds_line, left + 12, top - 22 - 5 * menu_line_h);

  {
    char trace_line[160];
    if (trace_x_locked) {
      snprintf(trace_line, sizeof(trace_line),
               "Trace X: verrouillee sur [%g, %g] (b:debloquer)", trace_gx_min,
               trace_gx_max);
    } else {
      snprintf(trace_line, sizeof(trace_line),
               "Trace X: libre, suit la vue (b:verrouiller)");
    }
    glColor3f(0.9f, 0.9f, 0.9f);
    graph_draw_text(trace_line, left + 12, top - 22 - 6 * menu_line_h);
  }

  int y = top - 22 - 8 * menu_line_h;

  /* Liste des fonctions */
  for (int i = 0; i < func_count; ++i) {
    if (y < bottom + 22)
      break; /* don't overflow */
    char index_label[16];
    snprintf(index_label, sizeof(index_label), "[%2d]", i);
    const float *col = palette[i % PALETTE_COUNT];
    if (i == menu_selected) {
      /* Met en surbrillance l'entrée sélectionnée : dessine un quad translucide
       * derrière le texte puis change la couleur du texte en jaune clair. Les
       * offsets sont ajustés pour centrer visuellement la surbrillance autour
       * de la ligne de base du texte. */
      glColor4f(1.0f, 1.0f, 1.0f, 0.08f);
      int ly = y - 4;
      glBegin(GL_QUADS);
      glVertex2f(left + 4, ly + 9);
      glVertex2f(left + 4, ly - menu_line_h + 9);
      glVertex2f(right - 4, ly - menu_line_h + 9);
      glVertex2f(right - 4, ly + 9);
      glEnd();
    } else {
      glColor3f(0.9f, 0.9f, 0.9f);
    }

    /* Le numéro est coloré avec la palette partagée, le texte reste neutre. */
    glColor3f(col[0], col[1], col[2]);
    graph_draw_text_current_color(index_label, left + 12, y - menu_line_h / 2);

    glColor3f(0.9f, 0.9f, 0.9f);
    if (i == menu_selected)
      glColor3f(1.0f, 1.0f, 0.3f);
    graph_draw_text(funcs[i], left + 12 + menu_text_width(index_label) + 8,
                    y - menu_line_h / 2);
    y -= menu_line_h;

    if (menu_error_present[i]) {
      if (y < bottom + 22)
        break;
      glColor3f(1.0f, 0.45f, 0.45f);
      char error_line[320];
      int wrapped_lines;
      snprintf(error_line, sizeof(error_line), "Erreur [%d] : %s", i,
               menu_error_messages[i]);
      /* Une erreur longue peut occuper plusieurs lignes : on la découpe pour
       * éviter qu'elle déborde sur les fonctions suivantes. */
      wrapped_lines = menu_draw_wrapped_text(error_line, left + 24,
                                             y - menu_line_h / 2, text_width);
      y -= wrapped_lines * menu_line_h;
    }
  }

  /* Zone de saisie / édition */
  if (menu_editing) {
    char prompt[MENU_MAX_FUNC_LEN + 32];
    if (menu_adding)
      snprintf(prompt, sizeof(prompt), "Ajout: %s", menu_input);
    else
      snprintf(prompt, sizeof(prompt), "Edition: %s", menu_input);
    /* On conserve une ligne de pied dédiée afin de ne jamais superposer la
     * saisie avec la liste des fonctions. */
    glColor3f(0.8f, 0.8f, 0.8f);
    graph_draw_text(prompt, left + 12, bottom + 24);
  }

  /* Edition des bornes (mode d'édition propre) */
  if (menu_edit_bounds_mode != 0) {
    char prompt[128];
    if (menu_edit_bounds_mode == 1)
      snprintf(prompt, sizeof(prompt), "Edition borne X (min max): %s",
               menu_bounds_input);
    else
      snprintf(prompt, sizeof(prompt), "Edition borne Y (min max): %s",
               menu_bounds_input);
    glColor3f(0.8f, 0.8f, 0.8f);
    graph_draw_text(prompt, left + 12, bottom + 24);
  }

  /* Note : `menu_input` est un buffer de taille fixe. La gestion de la saisie
   * dans `menu_handle_key` garantit qu'on ne dépasse jamais `MENU_MAX_FUNC_LEN`
   * et que `menu_input_pos` reste dans la plage. */

  glDisable(GL_BLEND);

  /* restaure les matrices */
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
}

void menu_draw_button(void) {
  if (menu_visible)
    return;
  /* petit bouton en haut à gauche */
  const int pad = 8;
  const int width = 72;
  const int height = 28;
  int left = pad;
  int top = g_win_h - pad;
  int right = left + width;
  int bottom = top - height;

  /* Prépare la projection orthographique en pixels */
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, g_win_w, 0, g_win_h, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /* Fond */
  if (menu_visible)
    glColor4f(0.15f, 0.25f, 0.35f, 0.95f);
  else
    glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
  glBegin(GL_QUADS);
  glVertex2f(left, bottom);
  glVertex2f(left, top);
  glVertex2f(right, top);
  glVertex2f(right, bottom);
  glEnd();

  /* Bordure */
  glColor3f(1.0f, 1.0f, 1.0f);
  glLineWidth(1.0f);
  glBegin(GL_LINE_LOOP);
  glVertex2f(left, bottom);
  glVertex2f(left, top);
  glVertex2f(right, top);
  glVertex2f(right, bottom);
  glEnd();

  /* Libellé */
  glColor3f(0.95f, 0.95f, 0.95f);
  graph_draw_text("Menu", left + 12, top - (height / 2) - 6);

  glDisable(GL_BLEND);

  /* restaure les matrices */
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
}

Arbre menu_get_cached_arbre(int idx) {
  if (idx < 0 || idx >= func_count)
    return NULL;
  return func_trees[idx];
}
