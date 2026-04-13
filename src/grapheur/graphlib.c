#include "graphlib.h"

// PFNGLWINDOWPOS2IPROC glWindowPos2i;

/*
 * graphlib.c - 2D drawing helpers for GraphitX
 *
 * Fournit des utilitaires pour :
 *  - initialiser une fenêtre GLUT et configurer OpenGL
 *  - appliquer une vue orthographique correspondant aux bornes logiques
 *  - dessiner axes, grille, graduations, points et lignes
 *  - dessin de texte bitmap en coordonnées fenêtres (pixels)
 *
 * La conversion entre coordonnées logiques (world) et pixels s'appuie sur
 * les fonctions du module `utils/world.h` et sur les variables globales
 * `gx_min/gx_max/gy_min/gy_max` exposées par ce projet.
 */

static float bg_r = 0.0f, bg_g = 0.0f, bg_b = 0.0f;

/* Retourne un pas "agréable" proche de raw_world_step : série 1,2,5 * 10^k. */
static float compute_nice_step(float raw_world_step) {
  if (raw_world_step <= 0.0f)
    return 0.0f;
  double e = floor(log10(raw_world_step));
  double base = pow(10.0, e);
  double f = raw_world_step / base;
  double nice;
  if (f < 1.5)
    nice = 1.0;
  else if (f < 3.0)
    nice = 2.0;
  else if (f < 7.0)
    nice = 5.0;
  else
    nice = 10.0;
  return (float)(nice * base);
}

/*
 * compute_nice_step:
 * Le but est de prendre une valeur brute (`raw_world_step`) et de la
 * rapprocher d'une valeur « agréable » lisible par l'utilisateur :
 * la série 1,2,5 * 10^k est classique pour des graduations (ticks).
 * Exemple: raw=0.3 -> nice=0.2 ; raw=350 -> nice=500.
 */

/* Choisit un pas efficace dans le monde donné basé sur un pas demandé et la
  vue. Si requested<=0, la fonction calcule un pas pour que les lignes majeures
  soient espacées d'environ `desired_pixels` pixels. Si requested>0 mais que
  l'espacement en pixels serait < min_pixels, elle calcule aussi un pas
  "agréable" plus grand. */
static float choose_step(float requested, float span_world, int win_pixels,
                         int desired_pixels, int min_pixels) {
  if (span_world <= 0.0f || win_pixels <= 0)
    return requested > 0.0f ? requested : 1.0f;

  float px_per_unit = (float)win_pixels / span_world;
  if (requested > 0.0f) {
    float pixel_spacing = requested * px_per_unit;
    if (pixel_spacing >= (float)min_pixels)
      return requested; /* ok tel quel */
    /* sinon on continue pour calculer un pas plus agréable et plus grand */
  }

  /* calcule un pas brut qui donnerait environ desired_pixels d'espacement */
  float raw = (span_world * (float)desired_pixels) / (float)win_pixels;
  float nice = compute_nice_step(raw);
  if (nice <= 0.0f)
    nice = requested > 0.0f ? requested : 1.0f;
  return nice;
}

/*
 * choose_step:
 * - `requested` : step demandé par l'appelant (<=0 pour auto)
 * - `span_world` : taille de la vue (gx_max-gx_min ou gy_max-gy_min)
 * - `win_pixels` : taille de la fenêtre en pixels (largeur ou hauteur)
 * - `desired_pixels` : espacement visé entre graduations majeures
 * - `min_pixels` : espacement minimum acceptable avant d'augmenter le pas
 *
 * Si `requested` est raisonnable en pixels, on le conserve. Sinon on
 * calcule un pas brut adapté à l'espace et on le rapproche d'une valeur
 * « nice » via `compute_nice_step`.
 */

/** Initialise une fenêtre GLUT et un contexte OpenGL.
 * @param argc pointeur vers argc du programme, transmis à GLUT
 * @param argv argv du programme, transmis à GLUT
 * @param w largeur de la fenêtre en pixels
 * @param h hauteur de la fenêtre en pixels
 * @param title titre de la fenêtre
 */
void graph_init_window(int *argc, char **argv, int w, int h,
                       const char *title) {
  glutInit(argc, argv);
  /*
   * Demande un mode d'affichage initial pour GLUT.
   * - `GLUT_SINGLE` demande un seul buffer avant ; pour une animation fluide,
   *   préférer `GLUT_DOUBLE` et échanger les buffers avec `glutSwapBuffers()`.
   * - `GLUT_RGB` sélectionne un buffer couleur RGB (pas
   * d'alpha/profondeur/stencil ici).
   */
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(w, h);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(title);
  /* Mémorise la taille initiale de la fenêtre et définit le viewport. */
  world_set_window_size(w, h);
  /*
   * glViewport fait la correspondance entre les coordonnées normalisées et les
   * pixels de la fenêtre, et doit rester synchronisé avec la taille de la
   * fenêtre (à mettre à jour lors d'un redimensionnement).
   */
  glViewport(0, 0, (w > 0) ? w : 1, (h > 0) ? h : 1);
  glClearColor(bg_r, bg_g, bg_b, 1.0f);
}

/*
 * graph_init_window:
 * Initialise GLUT et l'état OpenGL minimal nécessaire. Note:
 * - la configuration utilise `GLUT_SINGLE` (buffer avant) ; ceci peut
 *   suffire pour des démonstrations simples mais double buffering est
 *   préférable pour des animations sans scintillement.
 * - nous mémorisons la taille de fenêtre dans `world_set_window_size`
 *   pour permettre la conversion world<->pixels ailleurs.
 */

/** Définit la couleur de fond utilisée par le rendu.
 * Met aussi à jour l'état de la couleur de nettoyage GL.
 * @param r composante rouge (0..1)
 * @param g composante verte (0..1)
 * @param b composante bleue (0..1)
 */
void graph_set_background(float r, float g, float b) {
  bg_r = r;
  bg_g = g;
  bg_b = b;
  glClearColor(bg_r, bg_g, bg_b, 1.0f);
}

/** Applique la vue logique courante à OpenGL.
 * Définit une projection orthographique correspondant aux bornes du monde et
 * réinitialise la matrice modelview à l'identité.
 */
void graph_apply_view(void) {
  /* Passe à la matrice de projection (paramètres caméra/projection). */
  glMatrixMode(GL_PROJECTION);
  /* Réinitialise la matrice de projection à l'identité pour éviter d'accumuler
   * les transformations. */
  glLoadIdentity();
  /* Définit une projection orthographique correspondant à nos bornes logiques.
    left = gx_min, right = gx_max,
    bottom = gy_min, top = gy_max,
    near = -1, far = 1 (suffisant pour du rendu 2D). */
  glOrtho(gx_min, gx_max, gy_min, gy_max, -1.0f, 1.0f);
  /* Revenir à la matrice modelview pour les transformations d'objet et la
   * réinitialiser. */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/*
 * graph_apply_view:
 * Configure la projection orthographique pour que les coordonnées « world »
 * correspondent directement aux coordonnées OpenGL utilisées pour le rendu 2D.
 * Après cet appel, dessiner avec des coordonnées (x,y) dans [gx_min,gx_max]
 * et [gy_min,gy_max] place les primitives correctement dans la vue.
 */

/** Dessine les axes X et Y sur la vue logique courante. */
void graph_draw_axes(void) {
  /* S'assurer que les matrices projection/modelview correspondent à notre vue
   * logique. */
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

/** Dessine une grille régulière sur la vue logique.
 * @param x_step espacement entre les lignes verticales de la grille (unités
 * monde)
 * @param y_step espacement entre les lignes horizontales de la grille (unités
 * monde)
 */
void graph_draw_grid(float x_step, float y_step) {
  /* adapte les pas si besoin pour que la grille reste lisible lors des zooms */
  float span_x = gx_max - gx_min;
  float span_y = gy_max - gy_min;
  float major_x = choose_step(x_step, span_x, g_win_w, 100, 40);
  float major_y = choose_step(y_step, span_y, g_win_h, 80, 32);
  float minor_x = major_x / 5.0f;
  float minor_y = major_y / 5.0f;
  if (minor_x <= 0.0f)
    minor_x = major_x;
  if (minor_y <= 0.0f)
    minor_y = major_y;

  graph_apply_view();

  /* Dessiner les lignes de grille mineures (pâles) */
  glLineWidth(1.0f);
  glColor3f(0.18f, 0.18f, 0.18f);
  glBegin(GL_LINES);
  /*
   * On calcule `start_x` comme le premier multiple de `minor_x` inférieur
   * ou égal à `gx_min`. L'utilisation de floorf puis multiplication par
   * `minor_x` garantit l'alignement des lignes de grille sur des valeurs
   * numériques stables (par opposition à une boucle démarrant à gx_min
   * et ajoutant `minor_x`, ce qui aurait pu accumuler des erreurs flottantes).
   */
  float start_x = (int)floorf(gx_min / minor_x) * minor_x;
  for (float x = start_x; x <= gx_max; x += minor_x) {
    glVertex2f(x, gy_min);
    glVertex2f(x, gy_max);
  }
  float start_y = (float)((int)floorf(gy_min / minor_y)) * minor_y;
  for (float y = start_y; y <= gy_max; y += minor_y) {
    glVertex2f(gx_min, y);
    glVertex2f(gx_max, y);
  }
  glEnd();

  /* Dessiner les lignes de grille majeures (plus visibles/épaisses) */
  glLineWidth(2.0f);
  glColor3f(0.34f, 0.34f, 0.34f);
  glBegin(GL_LINES);
  start_x = (int)floorf(gx_min / major_x) * major_x;
  for (float x = start_x; x <= gx_max; x += major_x) {
    glVertex2f(x, gy_min);
    glVertex2f(x, gy_max);
  }
  start_y = (float)((int)floorf(gy_min / major_y)) * major_y;
  for (float y = start_y; y <= gy_max; y += major_y) {
    glVertex2f(gx_min, y);
    glVertex2f(gx_max, y);
  }
  glEnd();
}

/*
 * Notes sur graph_draw_grid:
 * - Les lignes mineures (minor) sont tracées plus discrètes que les majeures.
 * - `minor = major/5` fournit une subdivision en 5 segments ; ce ratio est
 *   un choix visuel habituel.
 * - Lors de zooms extrêmes, `choose_step` adapte `major_x`/`major_y` pour
 *   garder une lisibilité acceptable.
 */

/** Trace un nuage de points à partir d'un tableau de points.
 * @param pts tableau de points en coordonnées monde
 * @param n nombre de points
 * @param r,g,b composantes couleur (0..1)
 * @param size taille des points en pixels
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

/** Trace une polyligne connectée à travers les points donnés.
 * @param pts tableau de points en coordonnées monde
 * @param n nombre de points
 * @param r,g,b composantes couleur (0..1)
 * @param width épaisseur visuelle de la ligne en pixels
 */
void graph_plot_lines(const Point *pts, int n, float r, float g, float b,
                      float width) {
  if (!pts || n <= 0)
    return;

  graph_apply_view();
  glLineWidth(width);
  glColor3f(r, g, b);

  /* Dessine une polyligne mais saute les points non finis (NaN/inf). Lorsqu'on
    rencontre un point non fini, on ferme la bande courante. De plus, si le saut
    vertical entre deux points finis consécutifs est très grand (ex : une
    asymptote), on coupe la bande pour éviter de dessiner une ligne verticale.
  */
  bool strip_open = false;
  bool have_prev = false;
  float prev_y = 0.0f;
  /* seuil exprimé comme fraction de la hauteur de la vue ; si delta_y >
    frac*span_y, on considère qu'il s'agit d'une discontinuité */
  float span_y = gy_max - gy_min;
  float max_jump =
      (span_y > 0.0f) ? fabsf(span_y) * 0.5f : 0.0f; /* 50% of view */

  for (int i = 0; i < n; ++i) {
    float px = pts[i].x;
    float py = pts[i].y;
    if (!isfinite(px) || !isfinite(py)) {
      if (strip_open) {
        glEnd();
        strip_open = false;
        have_prev = false;
      }
      continue;
    }

    if (!strip_open) {
      /* Démarre une nouvelle primitive GL_LINE_STRIP. Une bande de lignes relie
       * chaque sommet au précédent ; on démarre une nouvelle bande à chaque
       * fois qu'on rencontre le premier point fini après une discontinuité. */
      glBegin(GL_LINE_STRIP);
      strip_open = true;
      glVertex2f(px, py);
      prev_y = py;
      have_prev = true;
      continue;
    }

    /* si un point fini précédent existe, vérifier s'il y a un saut important */
    if (have_prev && max_jump > 0.0f && fabsf(py - prev_y) > max_jump) {
      /* coupe la bande et en démarre une nouvelle à ce point */
      glEnd();
      /* Ferme la bande précédente pour éviter de dessiner un long connecteur
       * vertical et démarre immédiatement une nouvelle bande au point courant.
       */
      glBegin(GL_LINE_STRIP);
      glVertex2f(px, py);
      prev_y = py;
      have_prev = true;
      continue;
    }

    /* continuation normale */
    glVertex2f(px, py);
    prev_y = py;
    have_prev = true;
  }
  if (strip_open)
    glEnd();
}

/*
 * graph_plot_lines - explication détaillée :
 * - Le code ouvre/ferme explicitement des `GL_LINE_STRIP` pour gérer les
 *   discontinuités : points non finis (NaN/inf) et sauts verticaux très
 *   importants. Sans cela, la primitive OpenGL relierait des points très
 *   éloignés par une ligne droite gênante (ex: tracé d'une asymptote).
 * - `max_jump` est défini comme 50% de la hauteur de la vue ; c'est un
 *   compromis visuel pour couper les sauts trop grands.
 */

static void graph_draw_text_impl(const char *text, int x, int y,
                                 int preserve_color) {
  if (!text)
    return;

  if (!preserve_color)
    glColor3f(1.0f, 1.0f, 1.0f);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0.0, (double)g_win_w, 0.0, (double)g_win_h, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glRasterPos2i(x, y);

  // glWindowPos2i = (PFNGLWINDOWPOS2IPROC)glutGetProcAddress("glWindowPos2i");
  // glWindowPos2i(x, y);

  for (const char *p = text; *p; ++p)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)*p);

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

/** Dessine une chaîne de texte bitmap aux coordonnées fenêtre (pixels).
 * L'origine est en bas à gauche.
 * @param text chaîne terminée par un zéro
 * @param x abscisse en pixels (depuis la gauche)
 * @param y ordonnée en pixels (depuis le bas)
 */
void graph_draw_text(const char *text, int x, int y) {
  graph_draw_text_impl(text, x, y, 0);
}

void graph_draw_text_current_color(const char *text, int x, int y) {
  graph_draw_text_impl(text, x, y, 1);
}

/*
 * graph_draw_text :
 * - Place le curseur raster en pixel via `glWindowPos2i` puis émet les
 *   caractères bitmap avec `glutBitmapCharacter`.
 * - Note de portabilité : `glutGetProcAddress` est utilisé pour récupérer
 *   l'adresse de `glWindowPos2i`. Sur certaines anciennes implémentations
 *   cette fonction peut être absente ; ici on suppose qu'elle existe.
 */

/* Calcule la largeur en pixels d'une chaîne bitmap avec la police GLUT
 * courante. */
static int graph_text_width(const char *text) {
  if (!text)
    return 0;
  int w = 0;
  for (const char *p = text; *p; ++p)
    w += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, (int)*p);
  return w;
}

/* Dessine du texte centré horizontalement en x, avec la ligne de base en y
 * (pixels). */
static void graph_draw_text_centered(const char *text, int x, int y) {
  int w = graph_text_width(text);
  graph_draw_text(text, x - (w / 2), y);
}

/* Dessine du texte aligné à droite pour que son bord droit soit en x (pixels).
 */
static void graph_draw_text_right(const char *text, int x, int y) {
  int w = graph_text_width(text);
  graph_draw_text(text, x - w, y);
}

/**
 * Dessine du texte ancré près du coin supérieur gauche de la fenêtre.
 * Ajoute un petit décalage par rapport aux bords.
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

/**
 * Dessine du texte ancré près du coin supérieur droit de la fenêtre.
 * Ajoute un petit décalage par rapport aux bords.
 */
void graph_draw_text_top_right(const char *text) {
  if (!text)
    return;

  const int pad_x = 8;
  const int pad_y = 20; /* distance from top baseline */

  int win_x = g_win_w - pad_x;
  int win_y = g_win_h - pad_y;
  graph_draw_text_right(text, win_x, win_y);
}

/*Dessine les nombres des graduations des axes*/
void graph_draw_numbers(float x_step, float y_step) {
  /* adapt step sizes if needed */
  float span_x = gx_max - gx_min;
  float span_y = gy_max - gy_min;
  x_step = choose_step(x_step, span_x, g_win_w, 100, 40);
  y_step = choose_step(y_step, span_y, g_win_h, 80, 32);

  graph_apply_view();

  /* décalages et marges en pixels */
  const int label_offset_below_axis = 25;  /* pixels sous l'axe X */
  const int label_offset_left_of_axis = 8; /* pixels à gauche de l'axe Y */
  const int bottom_margin = 6;

  /* Labels de l'axe X : dessinés centrés horizontalement sur le tick,
   * verticalement sous l'axe X */
  int axis_x_px, axis_y_px;
  world_to_pixels(0.0f, 0.0f, &axis_x_px, &axis_y_px);

  float start_x = (int)floorf(gx_min / x_step) * x_step;
  for (float x = start_x; x <= gx_max; x += x_step) {
    if (x == 0.0f)
      continue;
    char nombre[64];
    snprintf(nombre, sizeof(nombre), "%g", x);
    int x_px, y_px;
    world_to_pixels(x, 0.0f, &x_px, &y_px);
    int draw_y;
    if (y_px >= 0 && y_px <= g_win_h) {
      draw_y = y_px - label_offset_below_axis;
      if (draw_y < bottom_margin)
        draw_y = bottom_margin;
    } else {
      /* axis not visible: place labels at bottom margin */
      draw_y = bottom_margin;
    }
    graph_draw_text_centered(nombre, x_px, draw_y);
  }

  /* Labels de l'axe Y : alignés à droite à gauche de l'axe Y (ou à la marge
   * gauche si l'axe n'est pas visible) */
  float start_y = (float)((int)floorf(gy_min / y_step)) * y_step;
  for (float y = start_y; y <= gy_max; y += y_step) {
    char nombre[64];
    snprintf(nombre, sizeof(nombre), "%g", y);
    int x_px, y_px;
    world_to_pixels(0.0f, y, &x_px, &y_px);
    int draw_x, draw_y;
    draw_y = y_px - 6; /* small vertical adjustment */
    if (x_px >= 0 && x_px <= g_win_w) {
      int text_w = graph_text_width(nombre);
      draw_x = x_px - text_w - label_offset_left_of_axis;
      if (draw_x < 4)
        draw_x = 4;
    } else {
      /* axis not visible: place labels at left margin */
      draw_x = 6;
    }

    if (y == 0)
      draw_y -= 15;
    graph_draw_text(nombre, draw_x, draw_y);
  }
}

/*
 * graph_draw_numbers - détails :
 * - `world_to_pixels` convertit un point logique en coordonnées fenêtres
 *   (pixel). Si l'axe n'est pas visible (p.ex. hors-fenêtre), les labels
 *   sont placés au bord (marges) pour rester lisibles.
 * - On centre horizontalement les labels X sur le tick et on aligne à droite
 *   les labels Y pour qu'ils n'empiètent pas sur l'axe.
 */

/*Dessine de petits traits pour graduer les axes*/
void graph_draw_grid_min_lines(float x_step, float y_step) {
  /* adapt major steps first */
  float span_x = gx_max - gx_min;
  float span_y = gy_max - gy_min;
  float major_x = choose_step(x_step, span_x, g_win_w, 100, 40);
  float major_y = choose_step(y_step, span_y, g_win_h, 80, 32);

  /* graduation mineure = majeure / 5 */
  float minor_x = major_x / 5.0f;
  float minor_y = major_y / 5.0f;
  if (minor_x <= 0.0f)
    minor_x = major_x;
  if (minor_y <= 0.0f)
    minor_y = major_y;

  graph_apply_view();
  glLineWidth(1.5f);
  glColor3f(0.85f, 0.85f, 0.85f);
  glBegin(GL_LINES);

  float tick_half_h =
      (gy_max - gy_min) * 0.005f; /* demi-hauteur des graduations verticales */
  float start_x = (int)floorf(gx_min / minor_x) * minor_x;
  for (float x = start_x; x <= gx_max; x += minor_x) {
    glVertex2f(x, -tick_half_h);
    glVertex2f(x, tick_half_h);
  }

  float tick_half_w = (gx_max - gx_min) *
                      0.005f; /* demi-largeur des graduations horizontales */
  float start_y = (float)((int)floorf(gy_min / minor_y)) * minor_y;
  for (float y = start_y; y <= gy_max; y += minor_y) {
    glVertex2f(-tick_half_w, y);
    glVertex2f(tick_half_w, y);
  }

  glEnd();
}

/*
 * graph_draw_grid_min_lines : dessine des petits traits de graduation autour
 * des axes en utilisant une taille relative à la vue. Les valeurs 0.005f sont
 * empiriques et donnent un rendu proportionnel lors de zoom/pan.
 */

void graph_draw_coords_top_right(float x, float y) {
  graph_apply_view();
  /* Affiche une petite lecture des coordonnées en haut à droite en espace
   * pixel. */
  char text[100];
  snprintf(text, sizeof(text), "x = %f, y = %f", x, y);
  graph_draw_text_top_right(text);
}

/*Dessine des lignes rouges des axes jusqu'aux coordonnées du point*/
void graph_draw_coords_red_lines(float x, float y) {
  graph_apply_view();
  glLineWidth(1.5f);
  glColor3f(1.0f, 0.0f, 0.0f);
  glBegin(GL_LINES);

  glVertex2f(x, 0.0f);
  glVertex2f(x, y);

  glVertex2f(0.0f, y);
  glVertex2f(x, y);

  glEnd();
}
